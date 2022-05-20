#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace sf;

const int P = 20; // высота игрового поля
const int Q = 10; // ширина игрового поля

int field[P][Q] = { 0 }; // игровое поле

// массив для тетрамино
int fig[7][4] =
{
	1,3,5,7, // I
	3,5,4,6, // S
	2,4,5,7, // Z
	2,3,4,5, // O
	2,3,5,7, // L
	3,5,7,6, // J
	3,5,4,7, // T
};

struct Koord
{
	int x;
	int y;
};
Koord a[4];
Koord b[4];

//*** проверка на выход за границы игрового поля и на наличие пересечения тетрамино с уже находящимися на поле ***//
bool check()
{
	for (int i = 0; i < 4; i++)
	{
		if (a[i].x < 0 || a[i].x >= Q || a[i].y >= P)
		{
			return 0;
		}
		else if (field[a[i].y][a[i].x])
		{
			return 0;
		}
	}
	return 1;
};

int main()
{
	srand(time(0));

	RenderWindow window(VideoMode(190, 395), ""); // размер окна, заголовок

	// создание и загрузка текстуры
	Texture texture;
	texture.loadFromFile("C:\\Users\\nasty\\Desktop\\ОПСУ курсовая\\tetris\\pictures\\kub.png");

	Sprite sprite(texture); // создание спрайта
	sprite.setTextureRect(IntRect(0, 0, 18, 18)); // вырезаем из спрайта отдельный квадратик размером 18х18 пикселей

	int Score = 0; // очки
	int s = 0;  // горизонтальное перемещение
	bool turn = 0;  // вращение
	int color = 1;  // цвет
	bool start = true;  // первое появления
	int m = rand() % 7;  // тип тетрамино

	float timer = 0;  // таймер
	float interval = 0.3;  // задержка падения

	Clock clock;  // часы (таймер)

	//*** главный цикл приложения. Выполняется, пока открыто окно ***//
	while (window.isOpen())
	{
		// получаем время, прошедшее с начала отсчета, переводим его в секунды
		float time = (clock.getElapsedTime().asSeconds()) / 1.2; //знаменатель для скорости падения 
		clock.restart();
		timer += time;

		// обработка очереди событий в цикле
		Event event;
		while (window.pollEvent(event))
		{
			// если нажали на «крестик» -> закрываем окно
			if (event.type == Event::Closed)
			{
				window.close();
			}

			if (event.type == Event::KeyPressed) // если нажали кнопку на клавиатуре
			{
				if (event.key.code == Keyboard::Up) // стрелка вверх
				{
					turn = true;
				}
				else if (event.key.code == Keyboard::Left) // стрелка влево
				{
					s = -1;
				}
				else if (event.key.code == Keyboard::Right) // стрелка вправо
				{
					s = 1;
				}
			}
		}

		// стрелка вниз -> ускорение падения тетрамино
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			interval = 0.05;
		}

		//*** перемещение по горизонтали ***//
		for (int i = 0; i < 4; i++)
		{
			b[i] = a[i];
			a[i].x += s;
		}
		if (!check())
		{
			for (int i = 0; i < 4; i++) // вышли за пределы поля -> возвращаем старые координаты
			{
				a[i] = b[i];
			}
		}

		//*** вращение ***//
		if (turn && m != 3)
		{
			for (int i = 0; i < 4; i++)
			{
				Koord p = a[i]; // задаем центр вращения
				for (int i = 0; i < 4; i++)
				{
					int x = a[i].y - p.y; //y-y0
					int y = a[i].x - p.x; //x-x0
					a[i].x = p.x - x;
					a[i].y = p.y + y;
				}
				if (!check()) // вышли за пределы поля -> возвращаем старые координаты
				{
					for (int i = 0; i < 4; i++)
					{
						a[i] = b[i];
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}


		//*** движение тетрамино вниз, цвет, тип, координаты нового тетрамино ***//
		if (timer > interval)
		{
			for (int i = 0; i < 4; i++)
			{
				b[i] = a[i];
				a[i].y += 1;
			}
			if (!check())
			{
				for (int i = 0; i < 4; i++)
				{
					field[b[i].y][b[i].x] = color;
				}
				color = 1 + rand() % 7;
				m = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = fig[m][i] % 2;
					a[i].y = fig[m][i] / 2;
				}
			}
			timer = 0;
		}

		//*** проверка на полную заполненную горизонтальную линию ***//
		int k = P - 1;
		for (int i = P - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < Q; j++)
			{
				if (field[i][j])
				{
					count++;
					if (count == 10)
					{
						Score++;  //*** подсчет очков ***//
						//cout << endl << Score << endl;
					}
				}
				field[k][j] = field[i][j];
			}
			if (count < Q)
			{
				k--;
			}
			if (i == 2)  //*** проверка на GAME OVER ***//
			{
				for (int j = 0; j < Q; j++)
				{
					if (field[i][j])
					{
						//cout << endl << "Game over!" << endl;
						//cout << endl << "Number of points scored: " << Score << endl << endl;
					
						RenderWindow window(VideoMode(190, 395), "");
						window.clear(Color::White);
						Font font;
						font.loadFromFile("C:\\Windows\\Fonts\\Verdana.ttf"); // загрузка шрифта
						Text text("", font, 30); // размер
						text.setFillColor(Color::Black); // цвет шрифта
						ostringstream gameScore;    //  переменная очков
						gameScore << Score;        // формирование строки

						text.setString("Game over"); // вывод "Game over"
						text.setPosition(10, 50);  // позиция текста
						window.draw(text); // изображение текста
						text.setString("Score: " + gameScore.str()); // задаем строку тексту и вызываем сформированную выше строку методом .str()
						text.setPosition(20, 90); // позиция текста
						window.draw(text); // изображение текста

						window.display();
						system("pause");
					}
				}
			}
		}

		if (start) // если появление тетрамино на поле первое
		{
			start = false;
			m = rand() % 7;
			for (int i = 0; i < 4; i++)
			{
				a[i].x = fig[m][i] % 2;
				a[i].y = fig[m][i] / 2;
			}
		}
		s = 0;
		turn = 0;
		interval = 0.3;

		//*** отрисовка ***//
		window.clear(Color::White); // задаем цвет фона (белый)
		for (int i = 0; i < P; i++) // статическая отрисовка
			for (int j = 0; j < Q; j++)
			{
				if (field[i][j] == 0)
				{
					continue;
				}
				sprite.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
				sprite.setPosition(j * 18, i * 18);
				sprite.move(5, 31); // смещение
				window.draw(sprite);
			}

		for (int i = 0; i < 4; i++) //динамическая отрисовка
		{
			sprite.setTextureRect(IntRect(color * 18, 0, 18, 18)); // разукрашиваем тетрамино
			sprite.setPosition(a[i].x * 18, a[i].y * 18); // установка позиции каждого квадратика тетрамино
			sprite.move(5, 31); // смещение
			window.draw(sprite); //oтрисовка спрайта
		}

		Font font;
		font.loadFromFile("C:\\Windows\\Fonts\\Verdana.ttf"); 
		Text text("", font, 20);
		text.setFillColor(Color::Black);
		ostringstream gameScore;    
		gameScore << Score;       

		text.setString("Score: " + gameScore.str());
		text.setPosition(50, 50);
		window.draw(text);
		window.display(); 
	}
	return 0;
}
