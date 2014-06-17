//
//  RBCollisionObject.cpp
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

#include "RBCollisionObject.h"
#include "RBPhysicsWorld.h"

namespace RN
{
	namespace bullet
	{
		RNDefineMeta(CollisionObject, SceneNodeAttachment)
		
		CollisionObject::CollisionObject() :
			_collisionFilter(btBroadphaseProxy::DefaultFilter),
			_collisionFilterMask(btBroadphaseProxy::AllFilter),
			_owner(nullptr),
			_material(nullptr)
		{}
		
		CollisionObject::~CollisionObject()
		{
			if(_material)
			{
				_connection->Disconnect();
				_material->Release();
			}
		}
		
		
		void CollisionObject::SetCollisionFilter(short int filter)
		{
			_collisionFilter = filter;
			ReInsertIntoWorld();
		}
		void CollisionObject::SetCollisionFilterMask(short int mask)
		{
			_collisionFilterMask = mask;
			ReInsertIntoWorld();
		}
		void CollisionObject::SetMaterial(PhysicsMaterial *tmaterial)
		{
			if(_material)
			{
				_connection->Disconnect();
				_material->Release();
			}
			
			if((_material = SafeRetain(tmaterial)))
			{
				_connection = _material->signal.Connect(std::bind(&CollisionObject::UpdateFromMaterial, this, std::placeholders::_1));
				UpdateFromMaterial(_material);
			}
		}
		
		void CollisionObject::ReInsertIntoWorld()
		{
			if(_owner)
			{
				auto world = _owner;
				
				world->Lock();
				world->RemoveCollisionObject(this);
				world->InsertCollisionObject(this);
				world->Unlock();
			}
		}
		
		void CollisionObject::InsertIntoWorld(PhysicsWorld *world)
		{
			_owner = world;
		}
		void CollisionObject::RemoveFromWorld(PhysicsWorld *world)
		{
			_owner = nullptr;
		}
		
		
		void CollisionObject::DidUpdate(SceneNode::ChangeSet changeSet)
		{
			if(changeSet & SceneNode::ChangeSet::World)
			{
				World *world = GetParent()->GetWorld();
				
				if(!world && _owner)
				{
					_owner->RemoveCollisionObject(this);
					return;
				}
				
				if(world && !_owner)
				{
					PhysicsWorld::GetSharedInstance()->InsertCollisionObject(this);
					return;
				}
			}
		}
		
		void CollisionObject::DidAddToParent()
		{
			if(!_owner)
				PhysicsWorld::GetSharedInstance()->InsertCollisionObject(this);
		}
		
		void CollisionObject::WillRemoveFromParent()
		{
			if(_owner)
				_owner->RemoveCollisionObject(this);
		}
	}
}
