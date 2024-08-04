// Bảo đảm chỉ nạp file này một lần duy nhất.
#pragma once

#include <Windows.h> // Thư viện Windows API.
#include "offset.h"  // Thư viện chứa các offset (địa chỉ bộ nhớ).

// Khai báo các biến toàn cục.
extern bool modeESP;      // Biến toàn cục để bật/tắt chế độ ESP (hiển thị thông tin trên màn hình).
extern bool modeAutoAim;  // Biến toàn cục để bật/tắt chế độ AutoAim (tự động ngắm bắn).

// Cấu trúc Player chứa thông tin về người chơi.
struct Player {
	char unknown1[4];     // Dữ liệu chưa xác định (4 byte).
	float x;              // Tọa độ x của người chơi.
	float y;              // Tọa độ y của người chơi.
	float z;              // Tọa độ z của người chơi.
	char unknown2[0x24];  // Dữ liệu chưa xác định (36 byte).
	float yaw;            // Góc quay quanh trục đứng (hướng nhìn ngang).
	float pitch;          // Góc quay quanh trục ngang (hướng nhìn dọc).
	char unknown3[0x1C9]; // Dữ liệu chưa xác định (457 byte).
	char name[16];        // Tên người chơi.
	char unknown4[0xF7];  // Dữ liệu chưa xác định (247 byte).
	int team;             // Đội của người chơi.
	char unknown5[0x8];   // Dữ liệu chưa xác định (8 byte).
	int dead;             // Trạng thái chết của người chơi.
};

// Cấu trúc ViewMatrix chứa ma trận hiển thị.
struct ViewMatrix {
	// Các phần tử của ma trận hiển thị (4x4).
	float m11, m12, m13, m14; // Hàng thứ nhất (00, 01, 02, 03).
	float m21, m22, m23, m24; // Hàng thứ hai (04, 05, 06, 07).
	float m31, m32, m33, m34; // Hàng thứ ba (08, 09, 10, 11).
	float m41, m42, m43, m44; // Hàng thứ tư (12, 13, 14, 15).
};

// Hàm vẽ trên màn hình.
void Draw();

// Callback cho quy trình cửa sổ.
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Khởi tạo cửa sổ.
void InitWindow();

// Xóa cửa sổ.
void DeleteWindow();

// Khởi tạo người chơi.
void InitPlayer();

// Tính khoảng cách Euclidean giữa hai điểm.
float euclidean_distance(float, float);

// Kiểm tra xem người chơi có thể nhìn thấy mục tiêu không.
bool Seen(Player* target);

// Chế độ ESP (hiển thị thông tin người chơi lên màn hình).
void ESP();

// Chế độ AutoAim (tự động ngắm bắn).
void AutoAim();

// Cập nhật mục tiêu để ngắm bắn.
void UpdateAim();

// Hiển thị menu.
void Menu();
