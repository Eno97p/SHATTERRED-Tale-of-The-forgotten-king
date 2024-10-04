#pragma once
#include "Engine_Defines.h"


namespace Engine
{
    inline float RandomFloat(float min, float max)      //min �� max ������ flaot ���� �������ش�. ������ �Ұ���
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    inline int RandomInt(int min, int max)              //min�� max ������ int���� �������ش�. ���� �Ұ���
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    inline int RandomSign() //-1 Ȥ�� 1�� int���� ��ȯ����
    {
        float randomNumber = RandomFloat(-1.0f, 1.0f);
        if (randomNumber < 0)
            return -1;
        else
            return 1;
    }

    // _float3�� ���� ���ϱ� �Լ� ��ü
    struct Float3Add
    {
        _float3 operator()(const _float3& lhs, const _float3& rhs) const
        {
            return _float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }
    };

// _float3�� ���� ��Į�� ���� �Լ� ��ü
    struct Float3Multiply
    {
        _float3 operator()(const _float3& v, float scalar) const
        {
            return _float3(v.x * scalar, v.y * scalar, v.z * scalar);
        }
    };

    inline _float LerpFloat(_float fStart, _float fEnd, _float _t)
    {
        XMVECTOR startVec = XMVectorSet(fStart, 0.0f, 0.0f, 0.0f);
        XMVECTOR endVec = XMVectorSet(fEnd, 0.0f, 0.0f, 0.0f);
        XMVECTOR resultVec = XMVectorLerp(startVec, endVec, _t);
        _float result;
        XMStoreFloat(&result, resultVec);
        return result;
    }   
    
    inline _float SmoothStepFloat(_float t)
    {
        return t * t * (3 - 2 * t);
    }

    //matrix�� �������� ���ִ� �Լ�
    inline _matrix XMMatrixSlerp(_matrix matStart, _matrix matEnd, float _t)
    {
        // ��Ŀ��� ȸ�� �κ��� ���ʹϾ����� ����
        _vector qStart = XMQuaternionRotationMatrix(matStart);
        _vector qEnd = XMQuaternionRotationMatrix(matEnd);

        // ���ʹϾ� ���� ���� ����
        _vector qInterpolated = XMQuaternionSlerp(qStart, qEnd, _t);

        // ��Ŀ��� Right ����, Up ����, Look ���� ����
        _vector vStartRight = matStart.r[0];
        _vector vEndRight = matEnd.r[0];
        _vector vStartUp = matStart.r[1];
        _vector vEndUp = matEnd.r[1];
        _vector vStartLook = matStart.r[2];
        _vector vEndLook = matEnd.r[2];
        _vector vStartTranslation = matStart.r[3];
        _vector vEndTranslation = matEnd.r[3];

        // Right ����, Up ����, Look, Position ���� ���� ����
        _vector vInterpolatedRight = XMVectorLerp(vStartRight, vEndRight, _t);
        _vector vInterpolatedUp = XMVectorLerp(vStartUp, vEndUp, _t);
        _vector vInterpolatedLook = XMVectorLerp(vStartLook, vEndLook, _t);
        _vector vInterpolatedTranslation = XMVectorLerp(vStartTranslation, vEndTranslation, _t);

        // ������ ȸ��, �̵�, Right ����, Up ����, Look ���͸� �����Ͽ� ���� ��� ����
        _matrix matResult = XMMatrixRotationQuaternion(qInterpolated);
        matResult.r[0] = XMVector3Normalize(vInterpolatedRight);
        matResult.r[1] = XMVector3Normalize(vInterpolatedUp);
        matResult.r[2] = XMVector3Normalize(vInterpolatedLook);
        matResult.r[3] = vInterpolatedTranslation;

        return matResult;
    }
    inline _vector ReflectVector(const _vector& incidentDir, const _vector& normalDir)
	{
		return incidentDir - 2.0f * XMVector3Dot(incidentDir, normalDir) * normalDir;
	}

    inline string wstring_to_string(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);
        return strTo;
    }

    inline wstring string_to_wstring(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstrTo[0], size_needed);
        return wstrTo;
    }

    inline void save_wstring_to_stream(const std::wstring& wstr, std::ostream& os) {
        std::string utf8_string = wstring_to_string(wstr);
        std::size_t size = utf8_string.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        os.write(utf8_string.c_str(), size);
    }

    inline std::wstring load_wstring_from_stream(std::istream& is) {
        std::size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string utf8_string(size, '\0');
        is.read(&utf8_string[0], size);
        return string_to_wstring(utf8_string);
    }


    inline wstring const_char_to_wstring(const char* str) {
		std::string utf8_string = str;
		return string_to_wstring(utf8_string);
	}

    inline wchar_t* wstringToWchar(const wstring& wstr) {
        size_t len = wstr.length();
        wchar_t* wchars = new wchar_t[len + 1];
        errno_t err = wcsncpy_s(wchars, len + 1, wstr.c_str(), len);
        if (err != 0) {
            delete[] wchars;
            throw runtime_error("Error copying string");
        }
        return wchars;
    }




    //_float4, _float3, _float2 ==�����̶� != ���� �����ϵ��� �������̵�
    inline bool operator==(const XMFLOAT4& lhs, const XMFLOAT4& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    inline bool operator!=(const XMFLOAT4& lhs, const XMFLOAT4& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator==(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    inline bool operator!=(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator==(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline bool operator!=(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
        return !(lhs == rhs);
    }


    inline XMFLOAT4 XM3TO4(const XMFLOAT3& float3, float w = 1.0f)
    {
        return XMFLOAT4(float3.x, float3.y, float3.z, w);
    }


    //void CreatePhysXProcess(LPCWSTR  applicationName)
    //{
    //    STARTUPINFO si = {};
    //    PROCESS_INFORMATION pi = {};
    //    ZeroMemory(&si, sizeof(si));
    //    si.cb = sizeof(si);
    //    ZeroMemory(&pi, sizeof(pi));

    //    //= L"C:\\Program Files (x86)\\NVIDIA Corporation\\PhysX Visual Debugger 3\\PVD3.exe";
    //    LPWSTR commandLine = _wcsdup(applicationName);

    //    auto createProcessLambda = [&]() -> bool {
    //        return CreateProcess(
    //            applicationName,   // ������ ���α׷��� �̸�
    //            commandLine,       // ����� ����
    //            NULL,              // �⺻ ���μ��� ���� �Ӽ�
    //            NULL,              // �⺻ ������ ���� �Ӽ�
    //            FALSE,             // �ڵ� ��� ��Ȱ��ȭ
    //            0,                 // ���� �÷���
    //            NULL,              // �θ� ���μ��� ȯ�� ���� ���
    //            NULL,              // �θ� ���μ����� ���� ���丮 ���
    //            &si,               // STARTUPINFO ����ü
    //            &pi                // PROCESS_INFORMATION ����ü
    //        );
    //        };
    //    if (!createProcessLambda())
    //    {
    //        DWORD error = GetLastError();
    //        wprintf(L"CreateProcess failed (%d).\n", error);
    //    }
    //    else
    //    {
    //        // ���μ����� ������ �ڵ��� ����
    //        CloseHandle(pi.hProcess);
    //        CloseHandle(pi.hThread);
    //    }

    //    free(commandLine);


    //}


}

extern "C"
{
    void Free_String(wstring* wstrName)
    {
        wstrName->clear();


    }
}