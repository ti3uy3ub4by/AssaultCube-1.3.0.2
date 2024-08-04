#include <Windows.h>
#include <math.h>
#include "PlayerCheat.h"

// Định nghĩa hằng số PI và số lượng người chơi tối đa
#define M_PI 3.14159265358979323846
#define MAX_PLAYERS 100

// Khai báo các con trỏ và biến toàn cục
DWORD* player_base_ptr; // Con trỏ đến địa chỉ cơ bản của người chơi
DWORD* player_list; // Con trỏ đến danh sách người chơi
int* player_num; // Con trỏ đến số lượng người chơi

Player* player; // Con trỏ đến cấu trúc người chơi
ViewMatrix* view; // Con trỏ đến ma trận hiển thị

float closest_dis = -1.0f; // Khoảng cách gần nhất
float closest_yaw = -1.0f; // Góc quay ngang gần nhất
float closest_pitch = -1.0f; // Góc quay dọc gần nhất

float val_xhead[MAX_PLAYERS]; // Tọa độ x của đầu người chơi trên màn hình
float val_yhead[MAX_PLAYERS]; // Tọa độ y của đầu người chơi trên màn hình
float val_xfoot[MAX_PLAYERS]; // Tọa độ x của chân người chơi trên màn hình
float val_yfoot[MAX_PLAYERS]; // Tọa độ y của chân người chơi trên màn hình
bool is_enemy[MAX_PLAYERS]; // Trạng thái kẻ thù của người chơi
char name[MAX_PLAYERS][16]; // Tên người chơi

// Kích thước màn hình
int GameWidth;
int GameHeight;

HWND Window; // Handle đến cửa sổ chính
HWND NewWindow; // Handle đến cửa sổ mới
RECT rc; // Cấu trúc RECT để lưu kích thước cửa sổ
HDC hDC; // Handle đến Device Context
HDC memDC; // Handle đến bộ nhớ Device Context
HFONT font; // Handle đến font chữ
const COLORREF rgbRed = 0x000000FF; // Màu đỏ
const COLORREF rgbGreen = 0x0000FF00; // Màu xanh lá

bool modeESP = false; // Biến để bật/tắt chế độ ESP
bool modeAutoAim = false; // Biến để bật/tắt chế độ AutoAim

// Hàm khởi tạo người chơi
void InitPlayer() {
	// Địa chỉ cơ bản của người chơi
	player_base_ptr = (DWORD *)offset_Player_base_ptr;
	player = (Player*)(*player_base_ptr);
	player_list = (DWORD*)offset_Player_list;
	player_num = (int *)offset_Player_num;
	// Ma trận hiển thị
	view = (ViewMatrix*)offset_viewMatrix;
	// Kích thước màn hình
	GameWidth = *(int*)offset_GameWidth;
	GameHeight = *(int*)offset_GameHeight;
}

// Hàm tính khoảng cách Euclidean giữa hai điểm
float euclidean_distance(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

// Hàm ESP (hiển thị thông tin người chơi lên màn hình)
void ESP() {
	// Xóa dữ liệu cũ
	memset(val_xhead, 0, sizeof val_xhead);
	memset(val_yhead, 0, sizeof val_yhead);
	memset(val_xfoot, 0, sizeof val_xfoot);
	memset(val_yfoot, 0, sizeof val_yfoot);
	memset(is_enemy, false, sizeof is_enemy);
	memset(name, 0, sizeof name);
	for (int i = 1; i < *player_num; i++) {
		DWORD* target_offset = (DWORD*)(*player_list + (i * 4));
		Player* target = (Player*)(*target_offset);
		float dis = -1.0f;
		// Kiểm tra người chơi hợp lệ
		if (player != NULL && target != NULL && view != NULL && !target->dead && Seen(target)) {
			// Tọa độ đầu của người chơi
			float screenX = (view->m11 * target->x) + (view->m21 * target->y) + (view->m31 * target->z) + view->m41;
			float screenY = (view->m12 * target->x) + (view->m22 * target->y) + (view->m32 * target->z) + view->m42;
			float screenW = (view->m14 * target->x) + (view->m24 * target->y) + (view->m34 * target->z) + view->m44;

			// Tọa độ camera trên màn hình
			float camX = GameWidth / 2.0f;
			float camY = GameHeight / 2.0f;

			// Chuyển đổi tọa độ sang tọa độ màn hình
			float xhead = camX + (camX * screenX / screenW);
			float yhead = camY - (camY * screenY / screenW);

			// Tọa độ chân của người chơi
			screenX = (view->m11 * target->x) + (view->m21 * target->y) + (view->m31 * (target->z - 4.5)) + view->m41;
			screenY = (view->m12 * target->x) + (view->m22 * target->y) + (view->m32 * (target->z - 4.5)) + view->m42;
			screenW = (view->m14 * target->x) + (view->m24 * target->y) + (view->m34 * (target->z - 4.5)) + view->m44;

			// Chuyển đổi tọa độ sang tọa độ màn hình
			float xfoot = camX + (camX * screenX / screenW);
			float yfoot = camY - (camY * screenY / screenW);

			strcpy_s(name[i], target->name);
			val_xhead[i] = xhead;
			val_yhead[i] = yhead;
			val_xfoot[i] = xfoot;
			val_yfoot[i] = yfoot;
			is_enemy[i] = target->team == player->team ? false : true;
		}
	}
	UpdateWindow(NewWindow);
}

// Hàm kiểm tra xem người chơi có thể nhìn thấy mục tiêu không
bool Seen(Player* target) {
	// Tọa độ trên màn hình
	float screenX = (view->m11 * target->x) + (view->m21 * target->y) + (view->m31 * target->z) + view->m41;
	float screenY = (view->m12 * target->x) + (view->m22 * target->y) + (view->m32 * target->z) + view->m42;
	float screenW = (view->m14 * target->x) + (view->m24 * target->y) + (view->m34 * target->z) + view->m44;
	if (screenW <= 0.001f) {
		return false;
	}
	// Tọa độ camera trên màn hình
	float camX = GameWidth / 2.0f;
	float camY = GameHeight / 2.0f;

	// Chuyển đổi tọa độ sang tọa độ màn hình
	float deltax = (camX * screenX / screenW);
	float deltay = (camY * screenY / screenW);
	closest_dis = euclidean_distance(deltax, deltay);
	return true;
}

// Hàm AutoAim (tự động ngắm bắn)
void AutoAim() {
	// Khởi tạo
	closest_yaw = 0.0f;
	closest_pitch = 0.0f;

	// Khoảng cách gần nhất
	closest_dis = -1.0f;
	float current_dis = -1.0f;

	// Tìm người chơi gần nhất
	for (int i = 1; i < *player_num; i++) {
		DWORD* target_offset = (DWORD*)(*player_list + (i * 4));
		Player* target = (Player*)(*target_offset);

		// Kiểm tra người chơi hợp lệ
		if (player != NULL && target != NULL && player->team != target->team && !target->dead && view != NULL && Seen(target)) {
			float abspos_x = target->x - player->x;
			float abspos_y = target->y - player->y;
			float abspos_z = target->z + 0.3 - player->z; // Cộng 0.3 để ngắm vào đầu
			float Horizontal_distance = euclidean_distance(abspos_x, abspos_y); // Khoảng cách 2D
			// Cập nhật khoảng cách và góc quay
			if (current_dis == -1.0f || closest_dis < current_dis) {
				current_dis = closest_dis;
				// Góc quay ngang (yaw)
				float azimuth_xy = atan2f(abspos_y, abspos_x);
				float yaw = (float)(azimuth_xy * (180.0 / M_PI));
				closest_yaw = yaw + 90;

				// Góc quay dọc (pitch)
				float azimuth_z = atan2f(abspos_z, Horizontal_distance);
				closest_pitch = (float)(azimuth_z * (180.0 / M_PI));
			}
		}
	}

	// Cập nhật mục tiêu ngắm
	if (current_dis != -1.0f) {
		UpdateAim();
	}
}

// Hàm cập nhật mục tiêu ngắm
void UpdateAim() {
	player->yaw = closest_yaw;
	player->pitch = closest_pitch;
}

// Hàm callback cho quy trình cửa sổ
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_PAINT:
		Draw();
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

// Hàm khởi tạo cửa sổ
void InitWindow() {
	Window = FindWindowA("SDL_app", "AssaultCube");
	GetClientRect(Window, &rc);
	WNDCLASSEX wincl;
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.style = CS_HREDRAW | CS_VREDRAW;
	wincl.hInstance = 0;
	wincl.lpszClassName = (L"lookaway");
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.hIcon = NULL;
	wincl.hIconSm = NULL;
	wincl.hCursor = NULL;
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClassEx(&wincl);
	POINT point;
	point.x = rc.left;
	point.y = rc.top;
	ClientToScreen(Window, &point);
	NewWindow = CreateWindowEx(
		(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED),
		(L"lookaway"),
		(L"lookaway"),
		(WS_POPUP | WS_CLIPCHILDREN),
		point.x,
		point.y,
		GameWidth,
		GameHeight,
		0, 0, wincl.hInstance, 0
	);
	hDC = GetDC(NewWindow);
	COLORREF bkcolor = GetBkColor(hDC);
	SetLayeredWindowAttributes(NewWindow, bkcolor, 0, LWA_COLORKEY);
	ShowWindow(NewWindow, SW_SHOWNORMAL);
	UpdateWindow(NewWindow);
	memDC = CreateCompatibleDC(0);
	SetTextAlign(memDC, TA_CENTER | TA_NOUPDATECP);
	SetBkColor(memDC, RGB(0, 0, 0));
	SetBkMode(memDC, TRANSPARENT);
}

// Hàm vẽ lên màn hình
void Draw()
{
	HBITMAP hbitmap = CreateCompatibleBitmap(hDC, GameWidth, GameHeight);
	HGDIOBJ oldbitmap = SelectObject(memDC, hbitmap);
	BitBlt(memDC, 0, 0, GameWidth, GameHeight, 0, 0, 0, WHITENESS);
	for (int i = 1; i < *player_num; i++) {
		int offset = (val_yfoot[i] - val_yhead[i]) / 6;
		SelectObject(memDC, GetStockObject(DC_PEN));
		is_enemy[i] ? SetDCPenColor(memDC, rgbRed) : SetDCPenColor(memDC, rgbGreen);
		Rectangle(memDC, val_xhead[i] - 2 * offset, val_yhead[i] - offset, val_xfoot[i] + 2 * offset, val_yfoot[i]);

		is_enemy[i] ? SetTextColor(memDC, rgbRed) : SetTextColor(memDC, rgbGreen);
		SelectObject(memDC, font);
		TextOutA(memDC, val_xhead[i], val_yhead[i], name[i], strlen(name[i]));
	}
	BitBlt(hDC, 0, 0, GameWidth, GameWidth, memDC, 0, 0, SRCCOPY);
	DeleteObject(hbitmap);
}

// Hàm xóa cửa sổ
void DeleteWindow()
{
	DeleteDC(hDC);
	DeleteObject(memDC);
	DestroyWindow(NewWindow);
}

// Hàm menu (hiển thị menu)
// char STATE[2][4] = { "ON","OFF" };
// void Menu() {
// 	HBITMAP hbitmap = CreateCompatibleBitmap(hDC, GameWidth, GameHeight);
// 	HGDIOBJ oldbitmap = SelectObject(memDC, hbitmap);
// 	BitBlt(memDC, 0, 0, GameWidth, GameHeight, 0, 0, 0, WHITENESS);
//
// 	char strESP[] = "ESP Func is";
// 	modeESP ? (strcat_s(strESP, STATE[0]),SetTextColor(memDC, rgbGreen)) : (strcat_s(strESP, STATE[1]),SetTextColor(memDC, rgbRed));
// 	SelectObject(memDC, font);
// 	TextOutA(memDC, 10,10, strESP, strlen(strESP));
//
// 	char strAIM[] = "AutoAim Func is";
// 	modeAutoAim ? (strcat_s(strAIM, STATE[0]), SetTextColor(memDC, rgbGreen)) : (strcat_s(strAIM, STATE[1]), SetTextColor(memDC, rgbRed));
// 	SelectObject(memDC, font);
// 	TextOutA(memDC, 10, 30, strAIM, strlen(strAIM));
//
// 	BitBlt(hDC, 0, 0, GameWidth, GameWidth, memDC, 0, 0, SRCCOPY);
// 	DeleteObject(hbitmap);
// }
