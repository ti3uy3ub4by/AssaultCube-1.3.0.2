#include <Windows.h> // Thư viện Windows API.
#include <stdio.h>   // Thư viện chuẩn C để xử lý đầu vào/đầu ra.
#include "PlayerCheat.h" // Tệp tiêu đề chứa các khai báo và định nghĩa liên quan đến gian lận người chơi.

// Hàm kiểm tra và xử lý các phím chức năng.
void check()
{
	while (1) {
		// Menu(); // Hiển thị menu, có thể bỏ chú thích nếu cần.
		
		// Kiểm tra phím F1 để bật/tắt chế độ ESP.
		if (GetAsyncKeyState(VK_F1) & 1) {
			modeESP = !modeESP; // Chuyển đổi trạng thái của modeESP.
			if (modeESP == true) {
				InitWindow(); // Khởi tạo cửa sổ nếu chế độ ESP được bật.
			}
			else {
				DeleteWindow(); // Xóa cửa sổ nếu chế độ ESP bị tắt.
			}
		}

		// Nếu chế độ ESP đang bật, thực hiện chức năng ESP.
		if (modeESP) 
			ESP();
		
		// Kiểm tra phím F2 để bật/tắt chế độ AutoAim.
		if (GetAsyncKeyState(VK_F2) & 1) {
			modeAutoAim = !modeAutoAim; // Chuyển đổi trạng thái của modeAutoAim.
		}

		// Nếu chế độ AutoAim đang bật, thực hiện chức năng AutoAim.
		if (modeAutoAim)
			AutoAim();
		
		Sleep(1); // Tạm dừng 1ms để tránh tiêu tốn quá nhiều tài nguyên CPU.
	}
}

// Hàm DllMain là điểm vào chính của DLL.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	// Khi DLL được nạp vào một tiến trình.
	if (fdwReason == DLL_PROCESS_ATTACH) {
		InitPlayer(); // Khởi tạo người chơi.
		// Tạo một luồng mới để thực hiện hàm check.
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)check, NULL, 0, NULL);
	}
	return true; // Trả về true để thông báo DLL đã nạp thành công.
}
