// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� KEYHOOK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// KEYHOOK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef KEYHOOK_EXPORTS
#define KEYHOOK_API __declspec(dllexport)
#else
#define KEYHOOK_API __declspec(dllimport)
#endif

typedef BOOL (WINAPI *KeyboardCallback)(LPVOID lpContext, WPARAM wParam, LPARAM lParama);

// �������ӳ���
KEYHOOK_API BOOL StartHook(KeyboardCallback pfnCallback, LPVOID lpContext);


// ֹͣ���ӳ���
KEYHOOK_API BOOL StopHook(void);

