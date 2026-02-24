#include "pch.h"
#include "AuthStateManager.h"

AuthStateManager* AuthStateManager::m_pInstance = nullptr;

AuthStateManager::AuthStateManager() {
    m_currentState = STATE_WAITING_SCAN;
    m_strErrorMessage = L"";
}

AuthStateManager* AuthStateManager::GetInstance() {
    if (m_pInstance == nullptr) {
        m_pInstance = new AuthStateManager();
    }
    return m_pInstance;
}

void AuthStateManager::DestroyInstance() {
    if (m_pInstance) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void AuthStateManager::SetState(AuthState newState, CString strMsg) {
    if (m_currentState == newState && newState != STATE_ERROR) return;

    m_currentState = newState;
    m_strErrorMessage = strMsg;

    for (auto observer : m_observers) {
        if (observer) {
            observer->OnAuthStateChanged(m_currentState, m_strErrorMessage);
        }
    }
}

AuthState AuthStateManager::GetState() const {
    return m_currentState;
}

CString AuthStateManager::GetErrorMessage() const {
    return m_strErrorMessage;
}

void AuthStateManager::RegisterObserver(IAuthStateObserver* observer) {
    m_observers.push_back(observer);
}

void AuthStateManager::UnregisterObserver(IAuthStateObserver* observer) {
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void AuthStateManager::Reset() {
    SetState(STATE_WAITING_SCAN);
}