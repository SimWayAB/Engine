// ATOMIC

/// Defines one or more vertex buffers, an index buffer and a draw range.
class Geometry
{
	Vector<SharedPtr<VertexBuffer> > vertexBuffers_;	/// Vertex buffers.
	PODVector<unsigned> elementMasks_;					/// Vertex element masks.
	SharedPtr<IndexBuffer> indexBuffer_;				/// Index buffer.
	SharedArrayPtr<unsigned char> rawVertexData_;		/// Raw vertex data override.
	SharedArrayPtr<unsigned char> rawIndexData_;		/// Raw index data override.
	PrimitiveType primitiveType_;						/// Primitive type.
	unsigned indexStart_;								/// Start index.
	unsigned indexCount_;								/// Number of indices.
	unsigned vertexStart_;								/// First used vertex.
	unsigned vertexCount_;								/// Number of used vertices.
	unsigned positionBufferIndex_;						/// Index of vertex buffer with position data.
	unsigned rawVertexSize_;							/// Raw vertex data override size.
	unsigned rawElementMask_;							/// Raw vertex data override element mask.
	unsigned rawIndexSize_;								/// Raw index data override size.
	float lodDistance_;									/// LOD distance.
};

/// Source data for a 3D geometry draw call.
struct SourceBatch
{
	float distance_;					/// Distance from camera.
	Geometry* geometry_;				/// Geometry.
	SharedPtr<Material> material_;		/// Material.
	const Matrix3x4* worldTransform_;	/// World transform(s). For a skinned model, these are the bone transforms.
	unsigned numWorldTransforms_;		/// Number of world transforms.
	GeometryType geometryType_;			/// %Geometry type.
};

Drawable
{
	Vector<SourceBatch> batches_; /// Draw call source data.
};

/// 3D model resource. No material information!
class Model : public Resource
{
	BoundingBox boundingBox_;							/// Bounding box.
	Skeleton skeleton_;									/// Skeleton.
	Vector<SharedPtr<VertexBuffer> > vertexBuffers_;	/// Vertex buffers.
	Vector<SharedPtr<IndexBuffer> > indexBuffers_;		/// Index buffers.
	Vector<Vector<SharedPtr<Geometry> > > geometries_;	/// Geometries.
	Vector<PODVector<unsigned> > geometryBoneMappings_;	/// Geometry bone mappings.
	PODVector<Vector3> geometryCenters_;				/// Geometry centers.
	Vector<ModelMorph> morphs_;							/// Vertex morphs.
	PODVector<unsigned> morphRangeStarts_;				/// Vertex buffer morph range start.
	PODVector<unsigned> morphRangeCounts_;				/// Vertex buffer morph range vertex count.
	Vector<VertexBufferDesc> loadVBData_;				/// Vertex buffer data for asynchronous loading.
	Vector<IndexBufferDesc> loadIBData_;				/// Index buffer data for asynchronous loading.
	Vector<PODVector<GeometryDesc> > loadGeometries_;	/// Geometry definitions for asynchronous loading.
	// ATOMIC BEGIN
	Vector<String> geometryNames_;
	// ATOMIC END

	BeginLoad()
	{
		// Loads & fills vertex buffers & index buffers
		// Loads geometries
		// Loads morphs
		// Loads skeleton
		// Loads Bounding Box
		// Loads Geometry Centers
		// Loads Geometry names
	}
};

class StaticModel : public Drawable
{
	PODVector<StaticModelGeometryData> geometryData_;		/// Extra per-geometry data.
	Vector<Vector<SharedPtr<Geometry> > > geometries_;		/// All geometries.
	SharedPtr<Model> model_;								/// Model.
	unsigned occlusionLodLevel_;							/// Occlusion LOD level.
	mutable ResourceRefList materialsAttr_;					/// Material list attribute.

	// ATOMIC BEGIN
	mutable VariantVector geometryEnabled_;
	mutable bool geometryDisabled_;							/// true if any geometry has been disabled
	// ATOMIC END

	void SetModel(Model* model);
	void SetMaterial(Material* material);
	bool SetMaterial(unsigned index, Material* material);
};





/// Draw chain
void Graphics::Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount)
{
	glDrawElements(glPrimitiveType, indexCount, indexType, reinterpret_cast<const GLvoid*>(indexStart * indexSize));
}

void Geometry::Draw(Graphics* graphics)
{
	if (indexBuffer_ && indexCount_ > 0)
	{
		graphics->SetIndexBuffer(indexBuffer_);
		graphics->SetVertexBuffers(vertexBuffers_, elementMasks_);
		graphics->Draw(primitiveType_, indexStart_, indexCount_, vertexStart_, vertexCount_);
	}
	else if (vertexCount_ > 0)
	{
		graphics->SetVertexBuffers(vertexBuffers_, elementMasks_);
		graphics->Draw(primitiveType_, vertexStart_, vertexCount_);
	}
}

void Batch::Draw(View* view, bool allowDepthWrite) const
{
	if (!geometry_->IsEmpty())
	{
		Prepare(view, true, allowDepthWrite);	// All shader uniform handling
		geometry_->Draw(view->GetGraphics());
	}
}

void View::GetLightBatches()
{}

// Matrix
class Matrix3x4
{
	float m00_, m01_, m02_, m03_;
	float m10_, m11_, m12_, m13_;
	float m20_, m21_, m22_, m23_;
	//		 0,    0,    0,    1;
		 //X, Y, Z, Tx,
		 // X, Y, Z, Ty,
		 // X, Y, Z, Tz,

	/// Multiply a Vector3 which is assumed to represent position.
	Vector3 operator *(const Vector3& rhs) const
	{
		return Vector3(
			(m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_ + m03_),
			(m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_ + m13_),
			(m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_ + m23_)
			);
	}
	MINLINE CVec3 CMat4::operator*(const CVec3 &v) const
	{
		return CVec3(DOT3(mat[0],v.x, mat[4],v.y, mat[ 8],v.z) + mat[12],
			DOT3(mat[1],v.x, mat[5],v.y, mat[ 9],v.z) + mat[13],
			DOT3(mat[2],v.x, mat[6],v.y, mat[10],v.z) + mat[14]);
	}

};


