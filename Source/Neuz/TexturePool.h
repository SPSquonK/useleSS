
// gmpbigsun
// date : 2009_09_09
// texture pooling

#pragma  once 


typedef std::map< std::string, IDirect3DTexture9* >		Texture_Container;
typedef Texture_Container::iterator					Texture_Iter;

class TexturePool 
{
	// 모든 IDirect3DTexture9*를 관리하는 pool로 쓰기위해 만들어으나 기존의 데이터를 만지긴 싫다 -_-;
public:
	TexturePool( );
	~TexturePool( );

	static TexturePool* Get( );

	void Free( );
	IDirect3DTexture9* GetTexture( const std::string& dir, const std::string& filename );		//if not find, create the texture

protected:
	Texture_Container _cTextures;

};






