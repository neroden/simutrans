#ifndef label_h
#define label_h

#include "../simdings.h"
#include "../simimg.h"

class label_t : public ding_t
{
public:
	label_t(karte_t *welt, loadsave_t *file);
	label_t(karte_t *welt, koord3d pos, spieler_t *sp, const char *text);
	virtual ~label_t();

	void laden_abschliessen();

	void zeige_info();

#ifdef INLINE_DING_TYPE
#else
	typ get_typ() const { return ding_t::label; }
#endif

	image_id get_bild() const;
};

#endif
