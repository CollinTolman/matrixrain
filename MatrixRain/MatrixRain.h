#pragma once
#include "SFML/Graphics.hpp"
#include <map>

namespace mr
{

	constexpr int MR_FONTSIZE = 24;

	float Random(float  min = 0.0f, float max = 1.0f);

	struct Streamer
	{
		int X;
		float Y;
		int Length;
		float Speed;
	};

	class MatrixRain
	{
	public:
		
		MatrixRain();
		bool Start();

	private:

		enum class TextureMode
		{
			MatrixGreenPalette = 0,
			Color = 1
		};

		struct DataPointer
		{
			unsigned int Length;
			void * Data;
		};

		void InitStreamers();
		void InitCells();
		void InitWindow();
		bool LoadConfiguration();
		bool LoadTextures();
		bool LoadShader();
		void Update(float dt);
		void Render();
		void PrepareStreamer(Streamer& streamer);
		void Error(const std::string& error);
		
		bool LoadFile(const std::string filename, DataPointer& res);


		wchar_t RandomGlyph();

		sf::Shader m_Shader;
		sf::Texture m_FontTexture, m_Texture;
		sf::RenderWindow * m_Window;
		sf::Font m_Font;
		sf::RectangleShape * m_Cells;

		std::string m_SourceImageFile;
		TextureMode m_TextureMode;
		int m_Rows, m_Cols, m_CellSize;
		int m_StreamersCount;
		int m_ScreenWidth, m_ScreenHeight;
		
		std::map<wchar_t, sf::Glyph> m_Glyphs;
		std::vector<Streamer> m_Streamers;
	};
}

