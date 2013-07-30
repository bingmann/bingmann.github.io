// $Id: Picture.h 9 2006-08-08 08:29:15Z tb $

class Picture
{
public:

    // picture options which control the illustrator
    enum picoptions_t {
	// allow zooming of the view port
	FO_ZOOM 		= 1,

	// always fix the view port ratio
	FO_FIXRATIO		= 2,
    };

    // *** Pure Virtual Functions

    /// return an or-ed set of options to the picture
    virtual unsigned int getOptions() = 0;

    /// return the default view port when drawing the picture
    virtual Rect 	getDefaultViewport() = 0;

    /// reset all picture-defined values to their defaults
    virtual void 	resetVariables() = 0;

    /// draw the picture on the canvas
    virtual void	drawPicture(Canvas &c) = 0;
};
