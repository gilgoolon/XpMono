#include <Windows.h>

static void main_logic();

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	main_logic();
	return EXIT_SUCCESS;
}

static void main_logic()
{
	int x = 0;
	x++;
	x *= 3;
	x++;
}
