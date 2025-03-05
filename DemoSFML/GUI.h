#pragma once

#include "SFML/Graphics.hpp"

class GUIManager;

enum CORNER
{
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT
};

enum GUI_SIDE
{
	GUI_LEFT,
	GUI_TOP,
	GUI_RIGHT,
	GUI_BOTTOM
};

enum GUIContent
{
	IMAGE = 1 << 0,
	TEXT  = 1 << 1
};

inline GUIContent operator|(GUIContent a, GUIContent b)
{
	return static_cast<GUIContent>(static_cast<int>(a) | static_cast<int>(b));
}
inline GUIContent operator&(GUIContent a, GUIContent b)
{
	return static_cast<GUIContent>(static_cast<int>(a) & static_cast<int>(b));
}

class GUI
{
protected:
	sf::Vector2f m_topLeft;
	sf::Vector2f m_bottomRight;

	std::string m_className;
	std::string m_tag;

	sf::RectangleShape m_shape;

	int m_parent = 0;
	int m_id = 0;
	std::vector<int> m_children;

	sf::Color m_color;
	sf::Color m_hoverColor;

	bool m_hoverEnabled = false;
	bool m_enabled = true;
	bool m_ignoreClicks = false;

	std::vector<GUI*>* m_childrenBuffer = nullptr;

	GUIContent m_content = GUIContent(0);

	virtual void CalculateRectangleShape();
	virtual void SaveBaseData(std::ofstream&, GUIManager&); //SaveManagement
	virtual void Update(float, sf::RenderWindow&, bool, GUI*); //GameLoop
public:

#pragma region Constructors
	GUI(GUI&);
	GUI(int = -1);
	GUI(sf::Vector2f, sf::Vector2f, int = -1);
#pragma endregion

	virtual void RecalculateFamilyShapes();

#pragma region Getters
#pragma region Positions
	virtual sf::Vector2f GetPixelSize();
	virtual sf::Vector2f GetShapePosition();
	sf::Vector2f GetCornerPosition(CORNER);
	sf::Vector2f GetTopLeft() { return m_topLeft; }
	sf::Vector2f GetBottomRight() { return m_bottomRight; }
	virtual void GetTotalOffset(sf::Vector2f&);
	sf::RectangleShape* GetSfShape() { return &m_shape; }
#pragma endregion

#pragma region Colors
	sf::Color GetColor();
	sf::Color GetHoverColor() const;
	bool GetHoverEnabled() const;
#pragma	endregion

#pragma region Misc
	std::string GetTag(void) const;
	std::vector<int> GetChildren();
	int GetParent()const;
	std::string GetType();
	float GetNormalizedValue(GUI_SIDE);
	bool HasContentType(GUIContent);
	void GetFamilyList(std::vector<int>&);
	void PrepareResetID();
	int ResetID(int& _id);
	int GetID() const { return m_id; }
	bool IsEnabled() { return m_enabled; }
	bool GetIgnoreClicks() { return m_ignoreClicks; }
#pragma endregion
#pragma endregion

#pragma region Setters
#pragma region Positions
	void SetCornerPosition(sf::Vector2f, int);
	void SetSidePosition(sf::Vector2f, int);
#pragma endregion

#pragma region Colors
	void SetColor(sf::Color);
	void SetColorAlpha(float);
	void SetHoverColor(sf::Color);
	void SetHoverColorAlpha(float);
	void SetHoverEnabled(bool);
#pragma endregion

#pragma region Misc
	void SetTag(std::string);
	void SetNormalizedValue(GUI_SIDE, float);
	void RemoveChild(int); 
	void SetID(int);
	void SetEnabled(bool _enabled) { m_enabled = _enabled; }
	void SetIgnoreClicks(bool _ignore) { m_ignoreClicks = _ignore; }
#pragma endregion
#pragma endregion
	
	void SetParent(int _parent) { m_parent = _parent; }
	void AddChild(int);

#pragma region GameLoop
	virtual void Update(float, sf::RenderWindow&);
	virtual void Display(sf::RenderTarget&);
#pragma endregion

#pragma region Clicks
	bool DetectClick(sf::Vector2i, GUI*&);
	void Click();
#pragma endregion

#pragma region SaveManagement
	void Save(std::ofstream&, GUIManager&);
	void Load(std::ifstream&, GUIManager&, int _baseParent = -1);
	virtual void LoadCommand(std::string, std::ifstream&, GUIManager&, int _baseParent = -1);
#pragma endregion

#pragma region friends
	friend std::string& operator<<(std::string&, GUI&);

	friend class GUIManager;
	friend class GUIImage;
	friend class GUIText;
	friend class GUIGridLayout;
	#pragma endregion
};

typedef sf::Texture*(*TextureSearch)(std::string);

class GUIImage : public GUI
{
protected:
	std::string m_textureName;
	sf::Texture m_texture;
	void SaveBaseData(std::ofstream&, GUIManager&) override;
	
	void CalculateRectangleShape() override;
public:
	static TextureSearch TextureSearchFunction;

	GUIImage(GUI&);
	GUIImage(int = -1);
	GUIImage(std::string, int = -1);
	GUIImage(sf::Vector2f, sf::Vector2f, std::string, int = -1);

	void LoadCommand(std::string, std::ifstream&, GUIManager&, int _baseParent = -1) override;
	void ChangeTexture(std::string name);
	void ChangeTexture(const sf::Texture& _text);

	std::string GetTextureName()
	{
		return m_textureName;
	}
	sf::Texture* GetTexture() { return &m_texture; }
};

class GUIText : public GUIImage
{
protected:
	std::string m_script;
	sf::Text m_text;
	int m_fontSize;
	sf::Color m_fontColor;
	sf::Color m_fontHoverColor;
	sf::Vector2f m_margin;
	bool m_sizeUp = false;
	int m_fontSizeUpSize;

	void SaveBaseData(std::ofstream&, GUIManager&) override;
	void CalculateRectangleShape() override;
	void Update(float, sf::RenderWindow&, bool, GUI*) override;
public:
	static sf::Font* Font;

	GUIText(GUI&);
	GUIText(int = -1);
	GUIText(std::string, int = -1);
	GUIText(std::string, sf::Color = sf::Color::White, int = 60, int = -1);
	GUIText(std::string, std::string, sf::Color = sf::Color::White, int = 60, int = -1);

	void SetFontSize(int);
	int GetFontSize() const;

	void SetText(std::string);
	std::string GetText() const;

	sf::Text* GetSfText() { return &m_text; }

	void SetFontColor(sf::Color);
	sf::Color GetFontColor() const;

	sf::Color GetFontHoverColor() const;
	void SetFontHoverColor(sf::Color);

	void SetInMiddle(sf::RectangleShape& _shape);
	void SetMargin(sf::Vector2f);
	void SetMarginX(float);
	void SetMarginY(float);
	sf::Vector2f GetMargin() const;

	bool GetSizeUp() const;
	void SetSizeUp(bool);
	int GetFontSizeUpSize() const;
	void SetFontSizeUpSize(int);

	void UpdateText();

	void LoadCommand(std::string, std::ifstream&, GUIManager&, int _baseParent = -1) override;

	void Update(float, sf::RenderWindow&) override;
	void Display(sf::RenderTarget&) override;
};

class GUIGridLayout : public GUI
{
private:
	int m_padding[4];
	int m_horizontalSpacing;
	int m_verticalSpacing;

	int m_columnCount;
	int m_rowCount;

	void CalculateRectangleShape() override;
	void CalculateRectangleShapeWithFamily();
	void SaveBaseData(std::ofstream&, GUIManager&) override;

public:
	GUIGridLayout(GUI&);
	GUIGridLayout(int = -1);
	GUIGridLayout(sf::Vector2f, sf::Vector2f, int = -1);

	void RecalculateFamilyShapes() override;

	void LoadCommand(std::string, std::ifstream&, GUIManager&, int _baseParent = -1) override;
	void Display(sf::RenderTarget&) override;

	int GetPadding(GUI_SIDE) const;
	int GetHorizontalSpacing() const;
	int GetVerticalSpacing() const;
	int GetColumnCount() const;
	int GetRowCount() const;

	void SetPadding(GUI_SIDE, int);
	void SetHorizontalSpacing(int);
	void SetVerticalSpacing(int);
	void SetColumnCount(int);
	void SetRowCount(int);

};

class GUIMaskingContainer : public GUI
{
protected:
	sf::RenderTexture* m_renderTexture;
	sf::Vector2f m_offset;
	sf::View* m_view;

public:
	GUIMaskingContainer();
	GUIMaskingContainer(GUI&);
	void CalculateRectangleShape() override;

	sf::Vector2f GetOffset() const { return m_offset; }
	void SetOffsetX(float _x) { m_offset.x = _x; }
	void SetOffsetY(float _y) { m_offset.y = _y; }
	void SetOffset(sf::Vector2f _offset) { m_offset = _offset; }
	void SaveBaseData(std::ofstream&, GUIManager&) override;
	void LoadCommand(std::string, std::ifstream&, GUIManager&, int _baseParent = -1) override;

	void GetTotalOffset(sf::Vector2f& _offset) override;

	void Display(sf::RenderTarget&) override;
	sf::Vector2f GetShapePosition() override { return { 0,0 }; }
};