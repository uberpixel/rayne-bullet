//
//  RBRigidBody.cpp
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

#include "RBRigidBody.h"
#include "RBPhysicsWorld.h"

namespace RN
{
	namespace bullet
	{
		RNDefineMeta(RigidBody, CollisionObject)
		
		RigidBody::RigidBody(Shape *shape, float mass) :
			_shape(shape->Retain()),
			_rigidBody(nullptr)
		{
			Vector3 inertia = _shape->CalculateLocalInertia(mass);
			btVector3 btInertia = btVector3(inertia.x, inertia.y, inertia.z);
			
			btRigidBody::btRigidBodyConstructionInfo info(mass, this, _shape->GetBulletShape(), btInertia);
			
			_rigidBody = new btRigidBody(info);
			_rigidBody->setUserPointer(this);
		}
		
		RigidBody::RigidBody(Shape *shape, float mass, const Vector3 &inertia) :
			_shape(shape->Retain()),
			_rigidBody(nullptr)
		{
			btVector3 btInertia = btVector3(inertia.x, inertia.y, inertia.z);
			btRigidBody::btRigidBodyConstructionInfo info(mass, this, _shape->GetBulletShape(), btInertia);
			
			_rigidBody = new btRigidBody(info);
		}
		
		RigidBody::~RigidBody()
		{
			_shape->Release();
			delete _rigidBody;
		}
		
		
		RigidBody *RigidBody::WithShape(Shape *shape, float mass)
		{
			RigidBody *body = new RigidBody(shape, mass);
			return body->Autorelease();
		}
		RigidBody *RigidBody::WithShapeAndInertia(Shape *shape, float mass, const Vector3 &inertia)
		{
			RigidBody *body = new RigidBody(shape, mass, inertia);
			return body->Autorelease();
		}
		
		
		void RigidBody::SetMass(float mass)
		{
			Vector3 inertia = _shape->CalculateLocalInertia(mass);
			SetMass(mass, inertia);
		}
		void RigidBody::SetMass(float mass, const Vector3 &inertia)
		{
			_rigidBody->setMassProps(mass, btVector3(inertia.x, inertia.y, inertia.z));
		}
		void RigidBody::SetLinearVelocity(const Vector3 &velocity)
		{
			_rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
		}
		void RigidBody::SetAngularVelocity(const Vector3 &velocity)
		{
			_rigidBody->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
		}
		void RigidBody::SetCCDMotionThreshold(float threshold)
		{
			_rigidBody->setCcdMotionThreshold(threshold);
		}
		void RigidBody::SetCCDSweptSphereRadius(float radius)
		{
			_rigidBody->setCcdSweptSphereRadius(radius);
		}
		
		Vector3 RigidBody::GetLinearVelocity() const
		{
			const btVector3& velocity = _rigidBody->getLinearVelocity();
			return Vector3(velocity.x(), velocity.y(), velocity.z());
		}
		Vector3 RigidBody::GetAngularVelocity() const
		{
			const btVector3& velocity = _rigidBody->getAngularVelocity();
			return Vector3(velocity.x(), velocity.y(), velocity.z());
		}
		
		
		Vector3 RigidBody::GetCenterOfMass() const
		{
			const btVector3& center = _rigidBody->getCenterOfMassPosition();
			return Vector3(center.x(), center.y(), center.z());
		}
		Matrix RigidBody::GetCenterOfMassTransform() const
		{
			const btTransform& transform = _rigidBody->getCenterOfMassTransform();
			
			btQuaternion rotation = transform.getRotation();
			btVector3 position    = transform.getOrigin();
			
			Matrix matrix;
			
			matrix.Translate(Vector3(position.x(), position.y(), position.z()));
			matrix.Rotate(Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w()));
			
			return matrix;
		}
		
		
		void RigidBody::ApplyForce(const Vector3 &force)
		{
			_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
		}
		void RigidBody::ApplyForce(const Vector3 &force, const Vector3 &origin)
		{
			_rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(origin.x, origin.y, origin.z));
		}
		void RigidBody::ClearForces()
		{
			_rigidBody->clearForces();
		}
		
		void RigidBody::ApplyTorque(const Vector3 &torque)
		{
			_rigidBody->applyTorque(btVector3(torque.x, torque.y, torque.z));
		}
		void RigidBody::ApplyTorqueImpulse(const Vector3 &torque)
		{
			_rigidBody->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
		}
		void RigidBody::ApplyImpulse(const Vector3 &impulse)
		{
			_rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		}
		void RigidBody::ApplyImpulse(const Vector3 &impulse, const Vector3 &origin)
		{
			_rigidBody->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(origin.x, origin.y, origin.z));
		}
		
		
		
		
		void RigidBody::DidUpdate(SceneNode::ChangeSet changeSet)
		{
			CollisionObject::DidUpdate(changeSet);
			
			if(changeSet & SceneNode::ChangeSet::Position)
			{
				btTransform transform;
				
				getWorldTransform(transform);
				_rigidBody->setCenterOfMassTransform(transform);
			}
		}
		void RigidBody::UpdateFromMaterial(PhysicsMaterial *material)
		{
			_rigidBody->setFriction(material->GetFriction());
			_rigidBody->setRestitution(material->GetRestitution());
			_rigidBody->setDamping(material->GetLinearDamping(), material->GetAngularDamping());
		}
		
		
		void RigidBody::InsertIntoWorld(PhysicsWorld *world)
		{
			CollisionObject::InsertIntoWorld(world);
			
			{
				btTransform transform;
				
				getWorldTransform(transform);
				_rigidBody->setCenterOfMassTransform(transform);
			}
			
			auto bulletWorld = world->GetBulletDynamicsWorld();
			bulletWorld->addRigidBody(_rigidBody, GetCollisionFilter(), GetCollisionFilterMask());
		}
		
		void RigidBody::RemoveFromWorld(PhysicsWorld *world)
		{
			CollisionObject::RemoveFromWorld(world);
			
			auto bulletWorld = world->GetBulletDynamicsWorld();
			bulletWorld->removeRigidBody(_rigidBody);
		}
		
		
		void RigidBody::getWorldTransform(btTransform &worldTrans) const
		{
			if(!GetParent())
				return;
			
			Quaternion rotation = std::move(GetRotation());
			Vector3    position = std::move(GetPosition() - rotation.GetRotatedVector(offset));
			
			worldTrans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
			worldTrans.setOrigin(btVector3(position.x, position.y, position.z));
		}
		
		void RigidBody::setWorldTransform(const btTransform &worldTrans)8
		{
			if(!GetParent())
				return;
			
			btQuaternion rotation = worldTrans.getRotation();
			btVector3 position    = worldTrans.getOrigin();
			
			SetRotation(Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w()));
			SetPosition(Vector3(position.x(), position.y(), position.z()) + GetRotation().GetRotatedVector(offset));
		}
	}
}