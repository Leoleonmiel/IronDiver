//#ifndef SHADERS_MANAGER_HPP
//#define SHADERS_MANAGER_HPP
//
//#include "common.h"
//
//
//class ShadersManager
//{
//public:
//	ShadersManager();
//	~ShadersManager() = default;
//	
//
//	sf::RenderTexture* GetMainRT() const { return m_mainRT; }
//	sf::RenderTexture* GetEffectRT() const { return m_effectRT; }
//	sf::Sprite* GetMainSceneSprite() const { return m_mainSceneSprite; }
//	sf::Sprite* GetPlayerLightSprite() const { return m_playerLightSprite; }
//	std::map<std::string, sf::Shader*>& GetShaders() { return m_shaders; }
//	sf::Shader*& GetShader(const std::string& name) { return m_shaders[name]; }
//private:
//
//	sf::RenderTexture* m_mainRT = nullptr;
//	sf::RenderTexture* m_effectRT = nullptr;
//
//	sf::Sprite* m_mainSceneSprite = nullptr;
//	sf::Sprite* m_playerLightSprite = nullptr;
//
//	std::map<std::string, sf::Shader*> m_shaders;
//};
//
//
//#endif // !SHADERS_MANAGER_HPP
//
//
