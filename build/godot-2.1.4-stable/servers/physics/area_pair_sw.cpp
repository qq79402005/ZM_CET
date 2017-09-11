/*************************************************************************/
/*  area_pair_sw.cpp                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "area_pair_sw.h"
#include "collision_solver_sw.h"

bool AreaPairSW::setup(float p_step) {

	bool result =
			area_shape != -1 && body_shape != -1 &&
			area->test_collision_mask(body) &&
			CollisionSolverSW::solve_static(body->get_shape(body_shape), body->get_transform() * body->get_shape_transform(body_shape), area->get_shape(area_shape), area->get_transform() * area->get_shape_transform(area_shape), NULL, this);

	if (result != colliding) {

		if (result) {

			if (area->get_space_override_mode() != PhysicsServer::AREA_SPACE_OVERRIDE_DISABLED)
				body->add_area(area);
			if (area->has_monitor_callback())
				area->add_body_to_query(body, body_shape, area_shape);

		} else {

			if (area->get_space_override_mode() != PhysicsServer::AREA_SPACE_OVERRIDE_DISABLED)
				body->remove_area(area);
			if (area->has_monitor_callback())
				area->remove_body_from_query(body, body_shape, area_shape);
		}

		colliding = result;
	}

	return false; //never do any post solving
}

void AreaPairSW::solve(float p_step) {
}

void AreaPairSW::shift_shape_indices(const CollisionObjectSW *p_object, int p_removed_index) {

	if (p_object == body) {
		if (body_shape == p_removed_index)
			body_shape = -1;
		else if (body_shape > p_removed_index)
			body_shape--;
	} else if (p_object == area) {
		if (area_shape == p_removed_index)
			area_shape = -1;
		else if (area_shape > p_removed_index)
			area_shape--;
	}
}

AreaPairSW::AreaPairSW(BodySW *p_body, int p_body_shape, AreaSW *p_area, int p_area_shape) {

	body = p_body;
	area = p_area;
	body_shape = p_body_shape;
	area_shape = p_area_shape;
	colliding = false;
	body->add_constraint(this, 0);
	area->add_constraint(this);
	if (p_body->get_mode() == PhysicsServer::BODY_MODE_KINEMATIC)
		p_body->set_active(true);
}

AreaPairSW::~AreaPairSW() {

	if (colliding) {

		if (area->get_space_override_mode() != PhysicsServer::AREA_SPACE_OVERRIDE_DISABLED)
			body->remove_area(area);
		if (area->has_monitor_callback())
			area->remove_body_from_query(body, body_shape, area_shape);
	}
	body->remove_constraint(this);
	area->remove_constraint(this);
}

////////////////////////////////////////////////////

bool Area2PairSW::setup(float p_step) {

	bool result =
			shape_a != -1 && shape_b != -1 &&
			area_a->test_collision_mask(area_b) &&
			CollisionSolverSW::solve_static(area_a->get_shape(shape_a), area_a->get_transform() * area_a->get_shape_transform(shape_a), area_b->get_shape(shape_b), area_b->get_transform() * area_b->get_shape_transform(shape_b), NULL, this);

	if (result != colliding) {

		if (result) {

			if (area_b->has_area_monitor_callback() && area_a->is_monitorable())
				area_b->add_area_to_query(area_a, shape_a, shape_b);

			if (area_a->has_area_monitor_callback() && area_b->is_monitorable())
				area_a->add_area_to_query(area_b, shape_b, shape_a);

		} else {

			if (area_b->has_area_monitor_callback() && area_a->is_monitorable())
				area_b->remove_area_from_query(area_a, shape_a, shape_b);

			if (area_a->has_area_monitor_callback() && area_b->is_monitorable())
				area_a->remove_area_from_query(area_b, shape_b, shape_a);
		}

		colliding = result;
	}

	return false; //never do any post solving
}

void Area2PairSW::solve(float p_step) {
}

void Area2PairSW::shift_shape_indices(const CollisionObjectSW *p_object, int p_removed_index) {

	if (p_object == area_a) {
		if (shape_a == p_removed_index)
			shape_a = -1;
		else if (shape_a > p_removed_index)
			shape_a--;
	} else if (p_object == area_b) {
		if (shape_b == p_removed_index)
			shape_b = -1;
		else if (shape_b > p_removed_index)
			shape_b--;
	}
}

Area2PairSW::Area2PairSW(AreaSW *p_area_a, int p_shape_a, AreaSW *p_area_b, int p_shape_b) {

	area_a = p_area_a;
	area_b = p_area_b;
	shape_a = p_shape_a;
	shape_b = p_shape_b;
	colliding = false;
	area_a->add_constraint(this);
	area_b->add_constraint(this);
}

Area2PairSW::~Area2PairSW() {

	if (colliding) {

		if (area_b->has_area_monitor_callback() && area_a->is_monitorable())
			area_b->remove_area_from_query(area_a, shape_a, shape_b);

		if (area_a->has_area_monitor_callback() && area_b->is_monitorable())
			area_a->remove_area_from_query(area_b, shape_b, shape_a);
	}

	area_a->remove_constraint(this);
	area_b->remove_constraint(this);
}