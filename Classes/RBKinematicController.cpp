//
//  RBKinematicController.cpp
//  rayne-bullet
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "RBKinematicController.h"
#include "RBPhysicsWorld.h"

namespace RN
{
	namespace bullet
	{
		RNDefineMeta(KinematicController, CollisionObject)
		
		KinematicController::KinematicController(Shape *shape, float stepHeight, Shape *ghostShape) :
			_shape(shape->Retain()), _ghostShape(ghostShape?ghostShape:shape)
		{
			_ghostShape->Retain();
			_ghost = new btPairCachingGhostObject();
			_ghost->setCollisionShape(_ghostShape->GetBulletShape());
			_ghost->setCollisionFlags(_ghost->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
			_ghost->setUserPointer(this);
			
			_controller = new btKinematicCharacterController(_ghost, static_cast<btConvexShape *>(_shape->GetBulletShape()), stepHeight);
			_controller->warp(btVector3(offset.x, offset.y, offset.z));
			_controller->setJumpSpeed(5.0f);
			_controller->setGravity(9.81f);
		}
		
		KinematicController::~KinematicController()
		{
			delete _ghost;
			delete _controller;
			_ghostShape->Release();
			_shape->Release();
		}
		
		
		void KinematicController::SetWalkDirection(const Vector3 &direction)
		{
			_controller->setWalkDirection(btVector3(direction.x, direction.y, direction.z));
		}
		void KinematicController::SetFallSpeed(float speed)
		{
			_controller->setFallSpeed(speed);
		}
		void KinematicController::SetJumpSpeed(float speed)
		{
			_controller->setJumpSpeed(speed);
		}
		void KinematicController::SetMaxJumpHeight(float maxHeight)
		{
			_controller->setMaxJumpHeight(maxHeight);
		}
		void KinematicController::SetMaxSlope(float maxSlope)
		{
			_controller->setMaxSlope(maxSlope);
		}
		void KinematicController::SetGravity(float gravity)
		{
			_controller->setGravity(gravity);
		}
		
		
		bool KinematicController::IsOnGround()
		{
			return _controller->canJump();
		}
		void KinematicController::Jump()
		{
			_controller->jump();
		}
		
		btCollisionObject *KinematicController::GetBulletCollisionObject()
		{
			return _ghost;
		}
		
		
		
		void KinematicController::Update(float delta)
		{
			btTransform transform = _ghost->getWorldTransform();
			btVector3 &position = transform.getOrigin();
			
			SetWorldPosition(Vector3(position.x(), position.y(), position.z()) + offset);
		}
		
		
		void KinematicController::DidUpdate(SceneNode::ChangeSet changeSet)
		{
			CollisionObject::DidUpdate(changeSet);
			
			if(changeSet & SceneNode::ChangeSet::Position)
			{
				Vector3 position = GetWorldPosition() - offset;
				_controller->warp(btVector3(position.x, position.y, position.z));
			}
		}
		void KinematicController::UpdateFromMaterial(PhysicsMaterial *material)
		{
			_ghost->setFriction(material->GetFriction());
			_ghost->setRestitution(material->GetRestitution());
		}
		
		
		void KinematicController::InsertIntoWorld(PhysicsWorld *world)
		{
			CollisionObject::InsertIntoWorld(world);
			
			{
				Vector3 position = GetWorldPosition() - offset;
				_controller->warp(btVector3(position.x, position.y, position.z));
			}
			
			auto bulletWorld = world->GetBulletDynamicsWorld();
			
			bulletWorld->addCollisionObject(_ghost, GetCollisionFilter(), GetCollisionFilterMask());
			bulletWorld->addAction(_controller);
		}
		
		void KinematicController::RemoveFromWorld(PhysicsWorld *world)
		{
			CollisionObject::RemoveFromWorld(world);
			
			auto bulletWorld = world->GetBulletDynamicsWorld();
			
			bulletWorld->removeCollisionObject(_ghost);
			bulletWorld->removeAction(_controller);
		}
	}
}
