//
//  RBShape.cpp
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

#include "RBShape.h"

namespace RN
{
	namespace bullet
	{
		RNDefineMeta(Shape, Object)
		RNDefineMeta(SphereShape, Shape)
		RNDefineMeta(MultiSphereShape, Shape)
		RNDefineMeta(BoxShape, Shape)
		RNDefineMeta(CylinderShape, Shape)
		RNDefineMeta(CapsuleShape, Shape)
		RNDefineMeta(StaticPlaneShape, Shape)
		RNDefineMeta(TriangleMeshShape, Shape)
		RNDefineMeta(CompoundShape, Shape)
		
		Shape::Shape() :
			_shape(nullptr)
		{}
		
		Shape::Shape(btCollisionShape *shape) :
			_shape(shape)
		{}
		
		Shape::~Shape()
		{
			delete _shape;
		}
		
		
		
		Vector3 Shape::CalculateLocalInertia(float mass)
		{
			btVector3 inertia;
			_shape->calculateLocalInertia(mass, inertia);
			
			return Vector3(inertia.x(), inertia.y(), inertia.z());
		}
		
		void Shape::SetScale(const Vector3 &scale)
		{
			_shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		}
		
		
		
		SphereShape::SphereShape(float radius)
		{
			_shape = new btSphereShape(radius);
		}
		
		SphereShape *SphereShape::WithRadius(float radius)
		{
			SphereShape *shape = new SphereShape(radius);
			return shape->Autorelease();
		}
		
		
		MultiSphereShape::MultiSphereShape(const Vector3 *positions, float *radii, int count)
		{
			btVector3 *btPositions = new btVector3[count];
			
			for(int i = 0; i < count; i ++)
				btPositions[i] = btVector3(positions[i].x, positions[i].y, positions[i].z);
			
			_shape = new btMultiSphereShape(btPositions, radii, count);
			delete [] btPositions;
		}
		
		MultiSphereShape *MultiSphereShape::WithHeight(float height, float width)
		{
			Vector3 positions[2] = { Vector3(0.0, height * 0.5f - width, 0.0f), Vector3(0.0f, -height * 0.5f + width, 0.0f) };
			float radii[2] = { width, width };
			
			MultiSphereShape *shape = new MultiSphereShape(positions, radii, 2);
			return shape->Autorelease();
		}
		
		
		BoxShape::BoxShape(const Vector3 &halfExtents)
		{
			_shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
		}
		
		BoxShape *BoxShape::WithHalfExtents(const Vector3 &halfExtents)
		{
			BoxShape *shape = new BoxShape(halfExtents);
			return shape->Autorelease();
		}
		
		
		CylinderShape::CylinderShape(const Vector3 &halfExtents)
		{
			_shape = new btCylinderShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
		}
		
		CylinderShape *CylinderShape::WithHalfExtents(const Vector3 &halfExtents)
		{
			CylinderShape *shape = new CylinderShape(halfExtents);
			return shape->Autorelease();
		}
		
		
		CapsuleShape::CapsuleShape(float radius, float height)
		{
			_shape = new btCapsuleShape(radius, height);
		}
		
		CapsuleShape *CapsuleShape::WithRadius(float radius, float height)
		{
			CapsuleShape *shape = new CapsuleShape(radius, height);
			return shape->Autorelease();
		}
		
		
		StaticPlaneShape::StaticPlaneShape(const Vector3 &normal, float constant)
		{
			_shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), constant);
		}
		
		StaticPlaneShape *StaticPlaneShape::WithNormal(const Vector3 &normal, float constant)
		{
			StaticPlaneShape *shape = new StaticPlaneShape(normal, constant);
			return shape->Autorelease();
		}
		
		
		TriangleMeshShape::TriangleMeshShape(Model *model)
		{
			_triangleMesh = new btTriangleMesh();
			
			size_t meshes = model->GetMeshCount(0);
			for(size_t i=0; i<meshes; i++)
			{
				Mesh *mesh = model->GetMeshAtIndex(0, i);
				AddMesh(mesh);
			}
			
			_shape = new btBvhTriangleMeshShape(_triangleMesh, true);
		}
		
		TriangleMeshShape::TriangleMeshShape(Mesh *mesh)
		{
			_triangleMesh = new btTriangleMesh();
			
			AddMesh(mesh);
			
			_shape = new btBvhTriangleMeshShape(_triangleMesh, true);
		}
		
		TriangleMeshShape::TriangleMeshShape(const Array *meshes)
		{
			_triangleMesh = new btTriangleMesh();
			
			meshes->Enumerate<Mesh>([&](Mesh *mesh, size_t index, bool &stop) {
				
				AddMesh(mesh);
				
			});
			
			_shape = new btBvhTriangleMeshShape(_triangleMesh, true);
		}
		
		TriangleMeshShape::~TriangleMeshShape()
		{
			delete _triangleMesh;
		}
		
		TriangleMeshShape *TriangleMeshShape::WithModel(Model *model)
		{
			TriangleMeshShape *shape = new TriangleMeshShape(model);
			return shape->Autorelease();
		}
		
		Vector3 TriangleMeshShape::CalculateLocalInertia(float mass)
		{
			return Vector3(0.0f, 0.0f, 0.0f);
		}
		
		void TriangleMeshShape::AddMesh(Mesh *mesh)
		{
			const MeshDescriptor *posdescriptor = mesh->GetDescriptorForFeature(MeshFeature::Vertices);
			const MeshDescriptor *inddescriptor = mesh->GetDescriptorForFeature(MeshFeature::Indices);
			const uint8 *pospointer = mesh->GetVerticesData<uint8>() + posdescriptor->offset;
			
			const Vector3 *vertex1;
			const Vector3 *vertex2;
			const Vector3 *vertex3;
			
			size_t stride = mesh->GetStride();
			
			switch(inddescriptor->elementSize)
			{
				case 1:
				{
					const uint8 *index = mesh->GetIndicesData<uint8>();
					const uint8 *end = index + mesh->GetIndicesCount();
					
					while(index != end)
					{
						vertex1 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						vertex2 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						vertex3 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						
						_triangleMesh->addTriangle(btVector3(vertex1->x, vertex1->y, vertex1->z), btVector3(vertex2->x, vertex2->y, vertex2->z), btVector3(vertex3->x, vertex3->y, vertex3->z));
					}
					
					break;
				}
					
				case 2:
				{
					const uint16 *index = mesh->GetIndicesData<uint16>();
					const uint16 *end = index + mesh->GetIndicesCount();
					
					while(index != end)
					{
						vertex1 = reinterpret_cast<const Vector3 *>(pospointer + stride * index[0]);
						vertex2 = reinterpret_cast<const Vector3 *>(pospointer + stride * index[1]);
						vertex3 = reinterpret_cast<const Vector3 *>(pospointer + stride * index[2]);
						
						index += 3;
						_triangleMesh->addTriangle(btVector3(vertex1->x, vertex1->y, vertex1->z), btVector3(vertex2->x, vertex2->y, vertex2->z), btVector3(vertex3->x, vertex3->y, vertex3->z));
					}
					
					break;
				}
					
				case 4:
				{
					const uint32 *index = mesh->GetIndicesData<uint32>();
					const uint32 *end = index + mesh->GetIndicesCount();
					
					while(index != end)
					{
						vertex1 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						vertex2 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						vertex3 = reinterpret_cast<const Vector3 *>(pospointer + stride * (*index ++));
						
						_triangleMesh->addTriangle(btVector3(vertex1->x, vertex1->y, vertex1->z), btVector3(vertex2->x, vertex2->y, vertex2->z), btVector3(vertex3->x, vertex3->y, vertex3->z));
					}
					
					break;
				}
			}
		}
		
		CompoundShape::CompoundShape()
		{
			_shape = new btCompoundShape();
		}
		
		CompoundShape::~CompoundShape()
		{
			for(auto shape : _shapes)
			{
				shape->Release();
			}
		}
		
		void CompoundShape::AddChild(Shape *shape, const RN::Vector3 &position, const RN::Quaternion &rotation)
		{
			btCompoundShape *compoundShape = static_cast<btCompoundShape *>(_shape);
			
			_shapes.push_back(shape->Retain());
			
			btTransform transform;
			transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
			transform.setOrigin(btVector3(position.x, position.y, position.z));
			compoundShape->addChildShape(transform, shape->GetBulletShape());
		}
	}
}
