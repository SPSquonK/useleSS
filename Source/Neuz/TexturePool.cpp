
//gmpbigsun

#include "StdAfx.h"


#include "vutil.h"

TexturePool::TexturePool( )
{
}

TexturePool::~TexturePool( )
{
	Free( );
}

TexturePool* TexturePool::Get( )
{
	static TexturePool texPool;
	return &texPool;
}

void TexturePool::Free( )
{
	for( Texture_Iter iter = _cTextures.begin(); iter != _cTextures.end(); ++iter )
	{
		IDirect3DTexture9* pTex = iter->second;
		SAFE_RELEASE( pTex );
	}

	_cTextures.clear();
}

IDirect3DTexture9* TexturePool::GetTexture( const std::string& dir, const std::string& filename )
{
	Texture_Iter iter =  _cTextures.find( filename );
	if( iter != _cTextures.end() )
		return iter->second;

	//create texture!

	assert( _pMyDevice );

	std::string fullname = dir + filename;
	IDirect3DTexture9* pNewTex = NULL;
	if( FAILED( LoadTextureFromRes( _pMyDevice, fullname.c_str(), &pNewTex ) ) )
	{
		assert( 0 && "cannot find texture name" );
		return NULL;
	}

	const auto rst =_cTextures.emplace(filename, pNewTex);
	assert( rst.second );

	return pNewTex;
}

