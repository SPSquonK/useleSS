USE [USELESS_ACCOUNT_DBF]
GO

-- Password is empty

EXEC [dbo].[usp_CreateNewAccount]
	@account = N'shiho',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;

EXEC [dbo].[usp_CreateNewAccount]
	@account = N'squonk',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	

EXEC [dbo].[usp_CreateNewAccount]
	@account = N'test',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	

EXEC [dbo].[usp_CreateNewAccount]
	@account = N'basis',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	
EXEC [dbo].[usp_CreateNewAccount]
	@account = N'lama',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	
EXEC [dbo].[usp_CreateNewAccount]
	@account = N'psy',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	
EXEC [dbo].[usp_CreateNewAccount]
	@account = N'helix',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;
	
	
EXEC [dbo].[usp_CreateNewAccount]
	@account = N'porygon',
	@pw = N'4d1677b3d55fd9c68e6baa7b1bd638d0'
	;

GO

UPDATE ACCOUNT_TBL_DETAIL
 SET m_chLoginAuthority = 'Z'
 WHERE account IN ('squonk', 'shiho')

GO