#include "MatrixRain.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include <fstream>
#include "Util.h"


sf::Color _Palette[16] =
{
	sf::Color(0x00, 0x00, 0x00, 0xFF),
	sf::Color(0x00, 0x11, 0x00, 0xFF),
	sf::Color(0x00, 0x22, 0x00, 0xFF),
	sf::Color(0x00, 0x33, 0x00, 0xFF),
	sf::Color(0x00, 0x44, 0x00, 0xFF),
	sf::Color(0x00, 0x55, 0x00, 0xFF),
	sf::Color(0x00, 0x66, 0x00, 0xFF),
	sf::Color(0x00, 0x77, 0x00, 0xFF),
	sf::Color(0x00, 0x88, 0x00, 0xFF),
	sf::Color(0x00, 0x99, 0x00, 0xFF),
	sf::Color(0x00, 0xAA, 0x00, 0xFF),
	sf::Color(0x00, 0xBB, 0x00, 0xFF),
	sf::Color(0x00, 0xCC, 0x00, 0xFF),
	sf::Color(0x88, 0xDD, 0x88, 0xFF),
	sf::Color(0xAA, 0xEE, 0xAA, 0xFF),
	sf::Color(0xCC, 0xFF, 0xCC, 0xFF)
};

std::string Trim(const std::string src)
{

	auto start = src.begin();

	while (start != src.end() && *start == ' ')
		++start;

	if (start == src.end())
		return "";

	auto end = src.begin() + (src.size() - 1);

	while (*end == ' ')
		--end;

	return std::string(start, end + 1);
}

mr::MatrixRain::MatrixRain()
{
}

bool mr::MatrixRain::Start()
{

	if (!LoadConfiguration())
	{
		return false;
	}

	InitWindow();

	if (!LoadTextures())
	{
		Error("Couldn't load the textures");
		return false;
	}

	if (!LoadShader())
	{
 		Error("Couldn't load the shader");
		return false;
	}

	InitCells();
	InitStreamers();

	auto prevTime = std::chrono::system_clock::now();
	

	m_Window->setMouseCursorVisible(false);

	while (m_Window->isOpen())
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = now - prevTime;
		prevTime = now;

		sf::Event evt;
		while (m_Window->pollEvent(evt))
		{
			if (evt.type == sf::Event::KeyPressed)
			{
				m_Window->close();
			}
		}

		m_Window->clear();

		// Draw stuff
		Update(delta.count());
		Render();

		m_Window->display();
	}

	return true;

}

void mr::MatrixRain::InitStreamers()
{
	for (int i = 0; i < m_StreamersCount; i++)
	{
		m_Streamers.push_back(Streamer());
		PrepareStreamer(m_Streamers[i]);
	}
}

void mr::MatrixRain::InitCells()
{
	m_Cells = new sf::RectangleShape[m_Rows * m_Cols];
	
	for (int y = 0; y < m_Rows; y++)
	{
		for (int x = 0; x < m_Cols; x++)
		{
			int i = y * m_Cols + x;
			m_Cells[i] = sf::RectangleShape(sf::Vector2f(m_CellSize / 4.0f * 3.0f, m_CellSize));
			m_Cells[i].setPosition(x * m_CellSize, y * m_CellSize);
			m_Cells[i].setFillColor(sf::Color::Black);
			m_Cells[i].setTextureRect(m_Glyphs[RandomGlyph()].textureRect);
			m_Cells[i].setTexture(&m_FontTexture);
		}
	}
}

void mr::MatrixRain::InitWindow()
{

	m_ScreenWidth = sf::VideoMode::getDesktopMode().width;
	m_ScreenHeight = sf::VideoMode::getDesktopMode().height;

	m_StreamersCount = (int)(m_Cols * 2.5f);
	m_CellSize = std::ceilf((float)m_ScreenWidth / m_Cols);
	m_Rows = std::ceilf((float)m_ScreenHeight / m_CellSize);

	m_Window = new sf::RenderWindow(sf::VideoMode(m_ScreenWidth, m_ScreenHeight), "Matrix Rain", sf::Style::Fullscreen);

}

bool mr::MatrixRain::LoadConfiguration()
{
	const std::string configFile = "res/config.txt";

	std::ifstream is;

	m_Cols = 150;
	m_TextureMode = TextureMode::MatrixGreenPalette;
	m_SourceImageFile = "";

	is.open(configFile);
		
	if (!is.is_open())
	{
		std::ofstream os;
		os.open(configFile);

		os << "Columns " << m_Cols << std::endl;
		os << "TextureMode " << (int)m_TextureMode << std::endl;
		os << "SourceImage " << m_SourceImageFile << std::endl;
		
		os.close();
	}
	else
	{
		while (!is.eof())
		{
			std::string token, value;

			is >> token;
			std::getline(is, value);

			if (token == "Columns")
			{
				m_Cols = std::atoi(value.c_str());
			}
			else if (token == "TextureMode")
			{
				m_TextureMode = (TextureMode)(std::atoi(value.c_str()));
			}
			else if (token == "SourceImage")
			{
				m_SourceImageFile = Trim(value);
			}
		}

		is.close();
	}

	return true;
}

bool mr::MatrixRain::LoadTextures()
{
	DataPointer pFont, pMask;

	if (!m_Texture.loadFromFile(m_SourceImageFile))
	{
		sf::Image img;
		img.create(128, 128, sf::Color::White);
		m_Texture.loadFromImage(img);
	}

	if (!LoadFile("res/consola.ttf", pFont))
	{
		return false;
	}

	if (!m_Font.loadFromMemory(pFont.Data, pFont.Length))
	{
		return false;
	}

	for (wchar_t ch = 0x3d5; ch <= 0x4e5; ch++)
	{
		m_Glyphs.insert(std::make_pair(ch, m_Font.getGlyph(ch, MR_FONTSIZE, false)));
	}

	m_FontTexture = m_Font.getTexture(MR_FONTSIZE);

	return true;
}

bool mr::MatrixRain::LoadShader()
{
	DataPointer vsData, fsData;

	if (!LoadFile("res/shader.vert", vsData))
	{
		return false;
	}

	if (!LoadFile(m_TextureMode == TextureMode::MatrixGreenPalette ? "res/shader_mask.frag" : "res/shader_color.frag", fsData))
	{
		return false;
	}

	std::string vsCode = std::string((char*)vsData.Data, vsData.Length);
	std::string fsCode = std::string((char*)fsData.Data, fsData.Length);

	if (!m_Shader.loadFromMemory(vsCode, fsCode))
	{
		return false;
	}

	return true;
}

void mr::MatrixRain::Update(float dt)
{

	static auto streamerSort = [](const Streamer& a, const Streamer& b) -> bool
	{
		return a.Y > b.Y;
	};

	// Swap some glyphs
	for (int i = 0; i < m_Cols * m_Rows; i++)
	{
		if (Random() < dt * 0.2) // 20% glyphs changed every second
		{
			m_Cells[i].setTextureRect(m_Glyphs[RandomGlyph()].textureRect);
		}
		
		// Set all colors to black
		m_Cells[i].setFillColor(sf::Color::Black);

	}



	// Push streamers
	for (int i = 0; i < m_StreamersCount; i++)
	{
		m_Streamers[i].Y += dt * m_Streamers[i].Speed;

		if (m_Streamers[i].Y - m_Streamers[i].Length >= m_Rows)
		{
			PrepareStreamer(m_Streamers[i]);
		}
	}


	// Sort streamers by decreasing Y
	std::sort(m_Streamers.begin(), m_Streamers.end(), streamerSort);

	// Update colors	
	for (int i = 0; i < m_StreamersCount; i++)
	{
		int x = m_Streamers[i].X;
		int y = (int)m_Streamers[i].Y;
		
		for (int l = 0; l < m_Streamers[i].Length; l++)
		{
			sf::Color color;
			if (m_TextureMode == TextureMode::MatrixGreenPalette) 
			{
				int colorIndex = (1.0f - (l / (float)m_Streamers[i].Length)) * 15;
				color = _Palette[colorIndex];
			}
			else
			{
				sf::Uint8 val = (sf::Uint8)(255 * (1.0f - (l / (float)m_Streamers[i].Length)));
				color = sf::Color(val, val, val, 255);
			}

			int index = (y - l) * m_Cols + x;
			if (index >= 0 && index < m_Rows * m_Cols)
			{
				m_Cells[index].setFillColor(color);
			}
		}
	}




}

void mr::MatrixRain::Render()
{
	sf::Shader::bind(&m_Shader);
	m_Shader.setUniform("in_FontTexture", m_FontTexture);
	m_Shader.setUniform("in_MaskTexture", m_Texture);
	m_Shader.setUniform("in_ScreenWidth", (float)m_ScreenWidth);
	m_Shader.setUniform("in_ScreenHeight", (float)m_ScreenHeight);
	
	for (int i = 0; i < m_Rows * m_Cols; i++)
	{
		m_Window->draw(m_Cells[i]);
	}
	sf::Shader::bind(0);
}

void mr::MatrixRain::PrepareStreamer(Streamer & streamer)
{
	streamer.Y = 0;
	streamer.X = (int)(Random() * m_Cols);
	streamer.Length = (int)Random(m_Rows / 10.0f, m_Rows / 4.0f * 3.0f);
	streamer.Speed = Random(m_Rows / 10.0f, m_Rows / 2.0f);
}

void mr::MatrixRain::Error(const std::string& error)
{
	std::cerr << "Error: " << error << std::endl;
}

bool mr::MatrixRain::LoadFile(const std::string filename, mr::MatrixRain::DataPointer& result)
{
	std::ifstream is;

	is.open(filename, std::ifstream::binary);

	if (!is.is_open())
	{
		std::cerr << "Can't open file: " << filename << std::endl;
		return false;
	}

	is.seekg(0, std::ifstream::end);
	unsigned int length = is.tellg();
	is.seekg(0, std::ifstream::beg);


	result.Data = new char[length];
	result.Length = length;

	is.read((char*)result.Data, result.Length);

	is.close();

	return true;
}

wchar_t mr::MatrixRain::RandomGlyph()
{
	return (wchar_t)((float)rand() / RAND_MAX * (0x4e5 - 0x3d5) + 0x3d5);
}

float mr::Random(float min, float max)
{
	return min + ((float)rand() / RAND_MAX * (max - min));
}
