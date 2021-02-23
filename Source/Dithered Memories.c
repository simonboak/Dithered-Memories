/***************************************
 *                                     *
 *  D I T H E R E D   M E M O R I E S  *
 *                                     *
 *       by Simon Boak, © 2021         *
 *       unimplementedtrap.com         *
 *                                     *
 *  The following is NOT an example    *
 *  of the right way to write a Mac    *
 *  application, but you are free to   *
 *  use any part of this for your own  *
 *  work. This code is the result of   *
 *  me learning just enough to get it  *
 *  working as I wanted.               *
 *                                     *
 ***************************************/

/*** Window Constants ***/
#define kMoveToFront		(WindowPtr)-1L
#define kEmptyString		"\p"
#define kEmptyTitle			kEmptyString
#define kVisible			true
#define kNoGoAway			false
#define kNilRefCon			(long)nil
#define kAnimationDelay		(long)1

/*** Animation Constants ***/
#define kSplashPicResID		128
#define kSunPicResID		129


/***********/
/* Globals */
/***********/

short 		gOldMBarHeight;
short		gHorizon;
short		gScreenBottom;
short		gScreenXCenter;
short		gGridSizeFar;
short		gGridSizeNear;
short		gGridOffset;
short		gHasDrawnVerticals;

/*************/
/* Functions */
/*************/

void	ToolBoxInit( void );
void	WindowInit( void );
void	MainLoop( void );
void	DrawSplash( void );
void	DrawSun( void );
void	DrawGrid( void );
void	CenterPict( PicHandle picture, Rect *destRectPtr );



/************* main *****************/

void main( void ) {
	ToolBoxInit();
	WindowInit();
	
	// These values assume a 9" compact Mac like it's 1984
	gHorizon = 171;
	gScreenBottom = 342;
	gScreenXCenter = 256;
	gGridSizeFar = 20;
	gGridSizeNear = 80;
	gGridOffset = 0;
	gHasDrawnVerticals = 0;
	
	MainLoop();
}


/********** ToolBoxInit ***********/

void ToolBoxInit( void ) {
	InitGraf( &thePort );
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs( nil );
	InitCursor();
}


/*********** WindowInit *********/

void WindowInit( void ) {
	Rect		totalRect, mBarRect;
	RgnHandle	mBarRgn;
	WindowPtr	window;
	
	gOldMBarHeight = MBarHeight;
	MBarHeight = 0;
	
	window = NewWindow(
		nil,
		&(screenBits.bounds),
		kEmptyTitle,
		kVisible,
		plainDBox,
		kMoveToFront,
		kNoGoAway,
		kNilRefCon
	);
	
	SetRect(
		&mBarRect,
		screenBits.bounds.left,
		screenBits.bounds.top,
		screenBits.bounds.right,
		screenBits.bounds.top + gOldMBarHeight
	);
	
	mBarRgn = NewRgn();
	RectRgn( mBarRgn, &mBarRect );
	UnionRgn( window->visRgn, mBarRgn, window->visRgn );
	DisposeRgn( mBarRgn );
	SetPort( window );
	FillRect( &(window->portRect), black);
}







/********** MainLoop ************/

void MainLoop( void ) {
	DrawSplash();
	while ( ! Button() ) {
		// wait until mouse click
	}
	
	HideCursor();
	DrawSun();
	DrawGrid();
	
	while ( ! Button() ) {
		gGridOffset += 4;
		DrawGrid();
	}
	
	// Quit nicely
	MBarHeight = gOldMBarHeight;
	FlushEvents( -1, 0 );
}




/********* DrawSplash ********/
void DrawSplash( void ) {
	Rect		pictureRect;
	WindowPtr	window;
	PicHandle	picture;
	
	window = FrontWindow();
	
	pictureRect = window->portRect;
	
	picture = GetPicture( kSplashPicResID );
	
	if ( picture == nil ) {
		SysBeep(10);
		ExitToShell();
	}
	
	CenterPict( picture, &pictureRect );
	DrawPicture( picture, & pictureRect );
}



/********* DrawSun *********/
void DrawSun( void ) {
	short		sunX, sunY, sunWidth, sunHeight;
	Rect		sunRect, winRect;
	WindowPtr	window;
	PicHandle	sunPic;
	
	sunPic = GetPicture(kSunPicResID);
	if (sunPic == nil) {
		SysBeep(10);
		ExitToShell();
	}
	
	window = FrontWindow();
	SetPort( window );	
	winRect = window->portRect;
	
	FillRect( &(window->portRect), black);
	
	// Get the size of the sun and position it accordingly
	sunRect = (**(sunPic)).picFrame;
	
	sunX = 0;
	sunY = 0;
	sunWidth = sunRect.right - sunRect.left;
	sunHeight = sunRect.bottom - sunRect.top;
	
	SetRect(
		&sunRect, 
		sunX, 
		sunY,
		sunX + sunWidth,
		sunY + sunHeight
	);
	
	DrawPicture(sunPic, &sunRect);
	
	
	// Now do the right side
	sunPic = GetPicture(kSunPicResID+1);
	if (sunPic == nil) {
		SysBeep(10);
		ExitToShell();
	}
	
	sunRect = (**(sunPic)).picFrame;
	
	sunX = 252;
	sunY = 0;
	sunWidth = sunRect.right - sunRect.left;
	sunHeight = sunRect.bottom - sunRect.top;
	
	SetRect(
		&sunRect, 
		sunX, 
		sunY,
		sunX + sunWidth,
		sunY + sunHeight
	);
	
	DrawPicture(sunPic, &sunRect);
}


/********** DrawGrid ***********/
void DrawGrid(void) {
	WindowPtr	window;
	Rect		winRect;
	short		gridXFar, gridXNear, gridY, gridYStep;
	long		finalTicks;

	
	window = FrontWindow();
	winRect = window->portRect;
	SetPort( window );
	
	PenPat( white );
	PenMode( patCopy );
	
	
	// Draw the horizon
	MoveTo(0, gHorizon-1);
	LineTo(511, gHorizon-1);
	
	// Horizontal grid lines;
	gridYStep = 1;
	while (gridYStep < 170) {
		// Rub out the previous line
		PenPat( black );
		PenMode( patCopy );
		gridY = (0.0059 * ((gridYStep+gGridOffset-4) * (gridYStep+gGridOffset-4)) + 171);
		
		MoveTo(0, gridY);
		LineTo(511, gridY);
		
		// And draw in the new one
		PenPat( dkGray );
		PenMode( patXor );
		gridY = (0.0059 * ((gridYStep+gGridOffset) * (gridYStep+gGridOffset)) + 171); // basically y = x^2
		
		MoveTo(0, gridY);
		LineTo(511, gridY);
		
		// Advance to the next grid item
		gridYStep = gridYStep + 20;
		
		// Slow down the animation
		Delay( kAnimationDelay, &finalTicks);
		
	}
	
	
	// Reset if we've moved a full grid
	if ( gGridOffset > 18 ) {
		gGridOffset = 0;
	}
	
	
	
	// Vertical grid lines
	gridXFar = gGridSizeFar / 2; /* offset by half */
	gridXNear = gGridSizeNear / 2;
	
	if ( gHasDrawnVerticals < 1 ) {
		PenPat( white );
		PenMode( patCopy );
		
		while (gridXFar < 512) {
			// Right side
			MoveTo(gScreenXCenter + gridXFar, gHorizon);
			LineTo(gScreenXCenter + gridXNear, gScreenBottom);
			
			// Left side
			MoveTo(gScreenXCenter - gridXFar, gHorizon);
			LineTo(gScreenXCenter - gridXNear, gScreenBottom);
			
			// Step to the next grid
			gridXFar = gridXFar + gGridSizeFar;
			gridXNear = gridXNear + gGridSizeNear;
		}
		
		gHasDrawnVerticals = 1;
	}
}


/******** CenterPict ********/
void CenterPict( PicHandle picture, Rect *destRectPtr ) {
	Rect	windRect, pictRect;
	
	windRect = *destRectPtr;
	pictRect = (**( picture )).picFrame;
	OffsetRect( &pictRect,
				windRect.left - pictRect.left,
				windRect.top - pictRect.top
	);
	OffsetRect( &pictRect,
				(windRect.right - pictRect.right)/2,
				(windRect.bottom - pictRect.bottom)/2
	);
	*destRectPtr = pictRect;
}