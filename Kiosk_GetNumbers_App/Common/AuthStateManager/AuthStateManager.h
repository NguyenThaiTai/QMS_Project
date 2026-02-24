#pragma once
#include <vector>
#include <algorithm>
#include <atlstr.h>

// add start auth state (state machine) enum NTTai 20260125
enum AuthState {
    STATE_WAITING_SCAN,  // Chờ người dùng (Đặt thẻ / Đặt ngón tay / Nhìn vào cam)
    STATE_SCANNING,      // Thiết bị đang hoạt động (Quét thẻ / Quét vân tay / Nhận diện mặt)
    STATE_VERIFYING,     // Đã có dữ liệu thô, đang kiểm tra logic (Check DB, Check Format)
    STATE_SUCCESS,       // Thành công (Hiện màu xanh)
    STATE_ERROR,         // Lỗi (Hiện màu đỏ, báo lý do)
    STATE_TIMEOUT        // Quá thời gian chờ (Tự động reset)
};
// add end auth state (state machine) enum NTTai 20260125

class IAuthStateObserver {
public:
    virtual void OnAuthStateChanged(AuthState newState, CString strMessage) = 0;
};

class AuthStateManager
{
private:
    static AuthStateManager* m_pInstance;
    AuthState m_currentState;
    CString m_strErrorMessage;
    std::vector<IAuthStateObserver*> m_observers;
    AuthStateManager();

public:
    static AuthStateManager* GetInstance();
    static void DestroyInstance();

    void SetState(AuthState newState, CString strMsg = L"");
    AuthState GetState() const;
    CString GetErrorMessage() const;

    void RegisterObserver(IAuthStateObserver* observer);
    void UnregisterObserver(IAuthStateObserver* observer);
    void Reset();
};