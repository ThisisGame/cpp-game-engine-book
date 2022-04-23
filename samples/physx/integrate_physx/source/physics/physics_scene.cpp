//
// Created by cp on 2022/4/23.
//

#include "physics_scene.h"
#include "utils/debug.h"

PhysicsScene::PhysicsScene(PxScene* px_scene) {}

PhysicsScene::~PhysicsScene() {}

void PhysicsScene::AddActor(PxActor& actor) {
    scene_->addActor(actor);
}
