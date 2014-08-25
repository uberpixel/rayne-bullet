//
//  RBShape.h
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

#ifndef __rayne_bullet__RBShape__
#define __rayne_bullet__RBShape__

#include <Rayne/Rayne.h>
#include <btBulletDynamicsCommon.h>

namespace RN
{
	namespace bullet
	{
		class Shape : public Object
		{
		public:
			Shape(btCollisionShape *shape);
			
			virtual Vector3 CalculateLocalInertia(float mass);
			void SetScale(const Vector3 &scale);
			
			btCollisionShape *GetBulletShape() const { return _shape; }
			
		protected:
			Shape();
			~Shape() override;
			
			btCollisionShape *_shape;
			
			RNDeclareMeta(Shape)
		};
		
		class SphereShape : public Shape
		{
		public:
			SphereShape(float radius);
			
			static SphereShape *WithRadius(float radius);
			
			RNDeclareMeta(SphereShape)
		};
		
		class MultiSphereShape : public Shape
		{
		public:
			MultiSphereShape(const Vector3 *positions, float *radii, int count);
			
			static MultiSphereShape *WithHeight(float height, float width);
			
			RNDeclareMeta(MultiSphereShape)
		};
		
		class BoxShape : public Shape
		{
		public:
			BoxShape(const Vector3 &halfExtents);
			
			static BoxShape *WithHalfExtents(const Vector3& halfExtents);
			
			RNDeclareMeta(BoxShape)
		};
		
		class CylinderShape : public Shape
		{
		public:
			CylinderShape(const Vector3 &halfExtents);
			
			static CylinderShape *WithHalfExtents(const Vector3 &halfExtents);
			
			RNDeclareMeta(CylinderShape)
		};
		
		class CapsuleShape : public Shape
		{
		public:
			CapsuleShape(float radius, float height);
			
			static CapsuleShape *WithRadius(float radius, float height);
			
			RNDeclareMeta(CapsuleShape)
		};
		
		class StaticPlaneShape : public Shape
		{
		public:
			StaticPlaneShape(const Vector3 &normal, float constant);
			
			static StaticPlaneShape *WithNormal(const Vector3 &normal, float constant);
			
			RNDeclareMeta(StaticPlaneShape)
		};
		
		class TriangleMeshShape : public Shape
		{
		public:
			TriangleMeshShape(Model *model);
			TriangleMeshShape(Mesh *mesh);
			TriangleMeshShape(const Array *meshes);
			
			~TriangleMeshShape() override;
			
			Vector3 CalculateLocalInertia(float mass) override;
			
			static TriangleMeshShape *WithModel(Model *model);
			
		private:
			void AddMesh(Mesh *mesh);
			btTriangleMesh *_triangleMesh;
			
			RNDeclareMeta(TriangleMeshShape)
		};
		
		class CompoundShape : public Shape
		{
		public:
			CompoundShape();
			~CompoundShape();
			void AddChild(Shape *shape, const RN::Vector3 &position, const RN::Quaternion &rotation);
			
		private:
			std::vector<Shape *> _shapes;
			
			RNDeclareMeta(CompoundShape)
		};
	}
}

#endif /* defined(__rayne_bullet__RBShape__) */
