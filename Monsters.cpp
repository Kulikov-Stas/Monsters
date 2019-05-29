#include <iostream>
#include <string>
#include <cstdlib> // для rand() и srand()
#include <ctime> // для time()

using namespace std;

// Генерируем рандомное число между min и max
int getRandomNumber(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	// равномерно распределяем генерацию числа в диапазоне
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

class Creature
{
protected:
	string m_name;
	char m_symbol;
	int m_health;
	int m_damage;
	int m_gold;

public:
	Creature(string name, char symbol, int health, int damage, int gold)
		: m_name{name}, m_symbol(symbol), m_health(health), m_damage(damage), m_gold(gold)
	{}

	string getName()
	{
		return m_name;
	}
	
	char getSymbol()
	{
		return m_symbol;
	}

	int getHealth()
	{
		return m_health;
	}

	int getDamage()
	{
		return m_damage;
	}

	int getGold()
	{
		return m_gold;
	}

	void reduceHealth(int health) //который уменьшает здоровье Creature на указанное целочисленное значение;
	{
		m_health -= health;
	}

	bool isDead() // который возвращает true, если здоровье Creature равно 0 или меньше;
	{
		return (m_health <= 0) ? true : false;
	}

	void addGold(int gold) // который добавляет золото Creature - у.
	{
		m_gold += gold;
	}
};

class Player: public Creature
{
	/*переменную - член level, которая начинается с 1;
	имя(пользователь вводит с клавиатуры);
	символ «@»;
	10 очков здоровья;
	1 очко урона(для начала);
	и 0 золота.
	Также напишите геттер для члена level и метод hasWon(), который возвращает true, если Player достиг 20 уровня.*/
private:
	int m_level = 1;
public:
	Player(string name)
		: Creature(name, '@', 10, 1, 0)
	{}

	void levelUp()
	{
		m_level++;
		m_damage++;
	}

	int getLevel()
	{
		return m_level;
	}

	bool hasWon()
	{
		return m_level >= 20;
	}
};

class Monster: public Creature
{
public:
	enum Type
	{
		DRAGON,
		ORC,
		SLIME,
		MAX_TYPES
	};

	struct MonsterData
	{
		const char* name;
		char symbol;
		int health;
		int damage;
		int gold;
	};

	static MonsterData monsterData[MAX_TYPES];

	Monster(Type type)
		: Creature(monsterData[type].name, monsterData[type].symbol, monsterData[type].health, monsterData[type].damage, monsterData[type].gold)
	{
	}

	static Monster getRandomMonster()
	{
		int num = getRandomNumber(0, MAX_TYPES - 1);
		return Monster(static_cast<Type>(num));
	}
};

Monster::MonsterData Monster::monsterData[Monster::MAX_TYPES]
{
	{ "dragon", 'D', 20, 4, 100 },
	{ "orc", 'o', 4, 2, 25 },
	{ "slime", 's', 1, 1, 10 }
};

// Этот метод обрабатывает атаку монстра игроком
void attackMonster(Player& p, Monster& m)
{
	// Если игрок мёртв, то он не может атаковать монстра
	if (p.isDead())
		return;

	std::cout << "You hit the " << m.getName() << " for " << p.getDamage() << " damage.\n";

	// Здоровье монстра уменьшается от урона игрока
	m.reduceHealth(p.getDamage());

	// Если монстр мёртв, увеличиваем level игрока
	if (m.isDead())
	{
		std::cout << "You killed the " << m.getName() << ".\n";
		p.levelUp();
		std::cout << "You are now level " << p.getLevel() << ".\n";
		std::cout << "You found " << m.getGold() << " gold.\n";
		p.addGold(m.getGold());
	}
}

// Этот метод обрабатывает атаку игрока монстром
void attackPlayer(Monster& m, Player& p)
{
	// Если монстр мёртв, то он не может атаковать игрока
	if (m.isDead())
		return;

	// Здоровье игрока уменьшается от урона монстра
	p.reduceHealth(m.getDamage());
	std::cout << "The " << m.getName() << " hit you for " << m.getDamage() << " damage.\n";
}

// Этот метод обрабатывает весь бой между игроком и рандомным монстром
void fightMonster(Player& p)
{
	// Сначала генерируем рандомного монстра
	Monster m = Monster::getRandomMonster();
	std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ").\n";

	// Пока монстр или игрок не мёртв, бой продолжается
	while (!m.isDead() && !p.isDead())
	{
		std::cout << "(R)un or (F)ight: ";
		char input;
		std::cin >> input;
		if (input == 'R' || input == 'r')
		{
			// 50/50 шанс удачного побега
			if (getRandomNumber(1, 2) == 1)
			{
				std::cout << "You successfully fled.\n";
				return; // встречу с монстром удалось избежать
			}
			else
			{
				// Неудачная попытка побега даёт монстру право атаковать
				std::cout << "You failed to flee.\n";
				attackPlayer(m, p);
				continue;
			}
		}

		if (input == 'F' || input == 'f')
		{
			// Сначала атакует игрок, затем монстр
			attackMonster(p, m);
			attackPlayer(m, p);
		}
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0))); // устанавливаем значение системных часов в качестве стартового числа
	rand(); // сбрасываем первый результат

	std::cout << "Enter your name: ";
	std::string playerName;
	std::cin >> playerName;

	Player p(playerName);
	std::cout << "Welcome, " << p.getName() << '\n';

	// Если игрок не мёртв и еще не победил, то игра продолжается
	while (!p.isDead() && !p.hasWon())
		fightMonster(p);

	// К этому моменту игрок либо мёртв, либо победил
	if (p.isDead())
	{
		std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
		std::cout << "Too bad you can't take it with you!\n";
	}
	else
	{
		std::cout << "You won the game with " << p.getGold() << " gold!\n";
	}

	return 0;
}