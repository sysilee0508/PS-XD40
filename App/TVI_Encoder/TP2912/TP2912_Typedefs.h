// *****************************************************
// Company : Techpoint Inc
// $Id:  $
// $Author:  $
// $Date:  $
// $Revision:  $
// $Log:  $
// ******************************************************

#ifndef	__TP2912_TYPEDEFS__
#define	__TP2912_TYPEDEFS__

#ifdef TASKINGC

#define DATA			_data
#define PDATA			_pdat
#define IDATA			idat
#define CODE			_rom
#define REENTRANT		_reentrant
#define	AT(atpos2)		_at( atpos2 )
#define CODE_P			_rom
#define PDATA_P			_pdat

#define INTERRUPT( num, name )	_interrupt(num) void name (void)

#elif defined KEILC

#define DATA		data
#define PDATA		pdata
#define IDATA		idata
#define CODE		code
#define CODE_P
#define PDATA_P
#define IDATA_P
#define DATA_P

#define INTERRUPT( num, name ) name() interrupt num

#endif

#define _between(x, a, b)	(a<=x && x<=b)
#define _swap(a, b)			{ a^=b; b^=a; a^=b; }

typedef	unsigned char	Register;
typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
typedef	unsigned long	DWORD;

//InputSource		

#endif	/* __TP2912_TYPEDEFS__ */
