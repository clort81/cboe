/*
 *  outdoors.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "location.h"

namespace legacy {
	struct out_wandering_type;
	struct outdoor_record_type;
};

struct cOutdoors {
public:
	class cWandering { // formerly out_wandering_type
	public:
		unsigned char monst[7];
		unsigned char friendly[3];
		short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
		short end_spec1,end_spec2;
	};
	unsigned char terrain[48][48];
	location special_locs[18];
	unsigned char special_id[18];
	location exit_locs[8];
	char exit_dests[8];
	location sign_locs[8];
	cWandering wandering[4],special_enc[4];
	location wandering_locs[4];
	Rect info_rect[8];
	unsigned char strlens[180];
	cSpecial specials[60];
	//char strs[120][256];
	char out_name[256];
	char rect_names[8][256];
	char comment[256];
	char spec_strs[90][256];
	char sign_strs[8][256];
	char(& out_strs(short i))[256];
	
	cOutdoors();
	cOutdoors& operator = (legacy::outdoor_record_type& old);
};