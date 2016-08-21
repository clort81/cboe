/*
 *  regtown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "regtown.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "scenario.hpp"
#include "strdlog.hpp"
#include "oldstructs.hpp"
#include "fileio.hpp"

void cTinyTown::append(legacy::tiny_tr_type& old, int){
	cField the_field, the_road;
	the_field.type = SPECIAL_SPOT;
	the_road.type = SPECIAL_ROAD;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(short i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(short j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(short i = 0; i < 32; i++)
		for(short j = 0; j < 32; j++) {
			ter[i][j] = old.terrain[i][j];
			light[i / 8][j] = old.lighting[i / 8][j];
			if(scenario->ter_types[ter[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario->ter_types[ter[i][j]].i == 3001) { // marker to indicate it used to be a road
				the_road.loc.x = i;
				the_road.loc.y = j;
				preset_fields.push_back(the_road);
			}
			// Convert roads that crossed grass/hill boundaries
			// It won't catch ones that sit exactly at the edge, though; in that case they'd need manual fixing
			// For simplicity we assume the non-hill space is either a city or a grass road
			// Terrain types used here:
			// 80 - grass road    81 - hill road
			// 38 - hill/grass    40 - hill|grass    42 - grass/hill    44 - grass|hill
			// where / means "over" and | means "beside"
			if(old.terrain[i][j] == 81 && i > 0 && i < 47 && j > 0 && j < 47) {
				if(old.terrain[i+1][j] == 81) {
					ter_num_t connect = old.terrain[i-1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 38;
				}
			}
			if(scenario->ter_types[ter[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; k < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					int found_spec_id = special_locs[found_spec].spec;
					cSpecial* node;
					if(!unused_special_slots.empty()) {
						int use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						node = &specials[use_slot];
						special_locs[found_spec].spec = use_slot;
					} else {
						special_locs[found_spec].spec = specials.size();
						specials.emplace_back();
						node = &specials.back();
					}
					node->type = eSpecType::IF_IN_BOAT;
					node->ex1b = -1; // any boat;
					node->ex1c = -1; // do nothing
					node->jumpto = found_spec_id; // else jump here
				}
			}
		}
	room_rect.resize(16);
	for(short i = 0; i < 16; i++) {
		room_rect[i].top = old.room_rect[i].top;
		room_rect[i].left = old.room_rect[i].left;
		room_rect[i].bottom = old.room_rect[i].bottom;
		room_rect[i].right = old.room_rect[i].right;
	}
	creatures.resize(30);
	for(short i = 0; i < 30; i++) {
		creatures[i].append(old.creatures[i]);
	}
}

void cMedTown::append(legacy::ave_tr_type& old, int){
	cField the_field, the_road;
	the_field.type = SPECIAL_SPOT;
	the_road.type = SPECIAL_ROAD;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(short i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(short j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			ter[i][j] = old.terrain[i][j];
			light[i / 8][j] = old.lighting[i / 8][j];
			if(scenario->ter_types[ter[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario->ter_types[ter[i][j]].i == 3001) { // marker to indicate it used to be a road
				the_road.loc.x = i;
				the_road.loc.y = j;
				preset_fields.push_back(the_road);
			}
			// Convert roads that crossed grass/hill boundaries
			// It won't catch ones that sit exactly at the edge, though; in that case they'd need manual fixing
			// For simplicity we assume the non-hill space is either a city or a grass road
			// Terrain types used here:
			// 80 - grass road    81 - hill road
			// 38 - hill/grass    40 - hill|grass    42 - grass/hill    44 - grass|hill
			// where / means "over" and | means "beside"
			if(old.terrain[i][j] == 81 && i > 0 && i < 47 && j > 0 && j < 47) {
				if(old.terrain[i+1][j] == 81) {
					ter_num_t connect = old.terrain[i-1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 38;
				}
			}
			if(scenario->ter_types[ter[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; k < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					int found_spec_id = special_locs[found_spec].spec;
					cSpecial* node;
					if(!unused_special_slots.empty()) {
						int use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						node = &specials[use_slot];
						special_locs[found_spec].spec = use_slot;
					} else {
						special_locs[found_spec].spec = specials.size();
						specials.emplace_back();
						node = &specials.back();
					}
					node->type = eSpecType::IF_IN_BOAT;
					node->ex1b = -1; // any boat;
					node->ex1c = -1; // do nothing
					node->jumpto = found_spec_id; // else jump here
				}
			}
		}
	room_rect.resize(16);
	for(short i = 0; i < 16; i++) {
		room_rect[i].top = old.room_rect[i].top;
		room_rect[i].left = old.room_rect[i].left;
		room_rect[i].bottom = old.room_rect[i].bottom;
		room_rect[i].right = old.room_rect[i].right;
	}
	creatures.resize(40);
	for(short i = 0; i < 40; i++) {
		creatures[i].append(old.creatures[i]);
	}
}

void cBigTown::append(legacy::big_tr_type& old, int){
	cField the_field, the_road;
	the_field.type = SPECIAL_SPOT;
	the_road.type = SPECIAL_ROAD;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(short i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(short j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(short i = 0; i < 64; i++)
		for(short j = 0; j < 64; j++) {
			ter[i][j] = old.terrain[i][j];
			light[i / 8][j] = old.lighting[i / 8][j];
			if(scenario->ter_types[ter[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario->ter_types[ter[i][j]].i == 3001) { // marker to indicate it used to be a road
				the_road.loc.x = i;
				the_road.loc.y = j;
				preset_fields.push_back(the_road);
			}
			// Convert roads that crossed grass/hill boundaries
			// It won't catch ones that sit exactly at the edge, though; in that case they'd need manual fixing
			// For simplicity we assume the non-hill space is either a city or a grass road
			// Terrain types used here:
			// 80 - grass road    81 - hill road
			// 38 - hill/grass    40 - hill|grass    42 - grass/hill    44 - grass|hill
			// where / means "over" and | means "beside"
			if(old.terrain[i][j] == 81 && i > 0 && i < 47 && j > 0 && j < 47) {
				if(old.terrain[i+1][j] == 81) {
					ter_num_t connect = old.terrain[i-1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						ter[i][j] = 38;
				}
			}
			if(scenario->ter_types[ter[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; k < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					int found_spec_id = special_locs[found_spec].spec;
					cSpecial* node;
					if(!unused_special_slots.empty()) {
						int use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						node = &specials[use_slot];
						special_locs[found_spec].spec = use_slot;
					} else {
						special_locs[found_spec].spec = specials.size();
						specials.emplace_back();
						node = &specials.back();
					}
					node->type = eSpecType::IF_IN_BOAT;
					node->ex1b = -1; // any boat;
					node->ex1c = -1; // do nothing
					node->jumpto = found_spec_id; // else jump here
				}
			}
		}
	room_rect.resize(16);
	for(short i = 0; i < 16; i++) {
		room_rect[i].top = old.room_rect[i].top;
		room_rect[i].left = old.room_rect[i].left;
		room_rect[i].bottom = old.room_rect[i].bottom;
		room_rect[i].right = old.room_rect[i].right;
	}
	creatures.resize(60);
	for(short i = 0; i < 60; i++) {
		creatures[i].append(old.creatures[i]);
	}
}

ter_num_t& cTinyTown::terrain(size_t x, size_t y){
	return ter[x][y];
}

void cTinyTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, ter, 32, 32);
}

void cTinyTown::readTerrainFrom(std::istream& file) {
	readArray(file, ter, 32, 32);
}

unsigned char& cTinyTown::lighting(size_t i, size_t r){
	return light[i][r];
}

ter_num_t& cMedTown::terrain(size_t x, size_t y){
	return ter[x][y];
}

void cMedTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, ter, 48, 48);
}

void cMedTown::readTerrainFrom(std::istream& file) {
	readArray(file, ter, 48, 48);
}

unsigned char& cMedTown::lighting(size_t i, size_t r){
	return light[i][r];
}

ter_num_t& cBigTown::terrain(size_t x, size_t y){
	return ter[x][y];
}

void cBigTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, ter, 64, 64);
}

void cBigTown::readTerrainFrom(std::istream& file) {
	readArray(file, ter, 64, 64);
}

unsigned char& cBigTown::lighting(size_t i, size_t r){
	return light[i][r];
}

cBigTown::cBigTown(cScenario& scenario) : cTown(scenario) {
	for(size_t i = 0; i < max_dim(); i++)
		for(size_t j = 0; j < max_dim(); j++) {
			terrain(i,j) = scenario.default_ground;
			lighting(i / 8,j) = 0;
		}
	init_start();
}

cMedTown::cMedTown(cScenario& scenario) : cTown(scenario) {
	for(size_t i = 0; i < max_dim(); i++)
		for(size_t j = 0; j < max_dim(); j++) {
			terrain(i,j) = scenario.default_ground;
			lighting(i / 8,j) = 0;
		}
	init_start();
}

cTinyTown::cTinyTown(cScenario& scenario) : cTown(scenario) {
	for(size_t i = 0; i < max_dim(); i++)
		for(size_t j = 0; j < max_dim(); j++) {
			terrain(i,j) = scenario.default_ground;
			lighting(i / 8,j) = 0;
		}
	init_start();
}

size_t cBigTown::max_dim() const {
	return 64;
}

size_t cMedTown::max_dim() const {
	return 48;
}

size_t cTinyTown::max_dim() const {
	return 32;
}
