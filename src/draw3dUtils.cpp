void drawBeam(Vec3 start, Vec3 end, float thickness, int color) {
	if (start.distance(end) < 0.001) return;
	Mesh *mesh = getMesh("assets/common/models/Cube.mesh");

	MeshProps props = newMeshProps();
	props.matrix = getBeamMatrix(start, end, thickness);
	Vec4 argb = hexToArgbFloat(color);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;

	drawMesh(mesh, props);
}

void drawCapsule(Capsule3 cap, int color);
void drawCapsule(Capsule3 cap, int color) {
	Mesh *mesh = getMesh("assets/common/models/Sphere.mesh");
	MeshProps props = newMeshProps();

	props.matrix.TRANSLATE(cap.start);
	props.matrix.SCALE(cap.radius);
	Vec4 argb = hexToArgbFloat(color);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);

	props = newMeshProps();
	props.matrix.TRANSLATE(cap.end);
	props.matrix.SCALE(cap.radius);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);

	mesh = getMesh("assets/common/models/Cylinder.mesh");
	props = newMeshProps();
	props.matrix = getBeamMatrix(cap.start, cap.end, cap.radius);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);
}

void drawBounds(AABB bounds, int color);
void drawBounds(AABB bounds, int color) {
	Vec3 size = bounds.max - bounds.min;
	Vec3 center = (bounds.min + bounds.max) / 2;
	Vec3 radius = size * 0.5;

	Mesh *mesh = getMesh("assets/common/models/Cube.mesh");

	MeshProps props = newMeshProps();
	props.matrix.TRANSLATE(center);
	props.matrix.SCALE(radius);
	Vec4 argb = hexToArgbFloat(color);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);
}

void drawCubeoid(Vec3 center, float size, int color) {
	drawCubeoid(center, v3(size, size, size), color);
}

void drawCubeoid(Vec3 center, Vec3 size, int color) {
	Vec3 radius = size * 0.5;

	Mesh *mesh = getMesh("assets/common/models/Cube.mesh");

	MeshProps props = newMeshProps();
	props.matrix.TRANSLATE(center);
	props.matrix.SCALE(radius);
	Vec4 argb = hexToArgbFloat(color);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);
}

void drawCubeoidOutline(Vec3 center, Vec3 size, float thickness, int color) {
	Vec3 radius = size * 0.5;
	Vec3 min = center - radius;
	Vec3 max = center + radius;
	drawBoundsOutline(min, max, thickness, color);
}

void drawSphere(Vec3 center, float size, int color) {
	Mesh *mesh = getMesh("assets/common/models/Sphere.mesh");

	MeshProps props = newMeshProps();
	props.matrix.TRANSLATE(center);
	props.matrix.SCALE(size);
	Vec4 argb = hexToArgbFloat(color);
	props.ambientLight = v3(argb.y, argb.z, argb.w) * argb.x;
	drawMesh(mesh, props);
}

void drawBoundsOutline(AABB bounds, float thickness, int color, Matrix4 matrix) {
	drawBoundsOutline(bounds.min, bounds.max, thickness, color, matrix);
}

void drawBoundsOutline(Vec3 min, Vec3 max, float thickness, int color, Matrix4 matrix) {
	Vec3 size = max - min;
	Vec3 p0 = min;

	Vec3 p1 = min;
	p1.x += size.x;

	Vec3 p2 = min;
	p2.y += size.y;

	Vec3 p3 = min;
	p3.x += size.x;
	p3.y += size.y;

	Vec3 p4 = p0;
	Vec3 p5 = p1;
	Vec3 p6 = p2;
	Vec3 p7 = p3;
	p4.z += size.z;
	p5.z += size.z;
	p6.z += size.z;
	p7.z += size.z;

	p0 = matrix * p0;
	p1 = matrix * p1;
	p2 = matrix * p2;
	p3 = matrix * p3;
	p4 = matrix * p4;
	p5 = matrix * p5;
	p6 = matrix * p6;
	p7 = matrix * p7;

	// drawCubeoid(p0, thickness, color);
	// drawCubeoid(p1, thickness, color);
	// drawCubeoid(p2, thickness, color);
	// drawCubeoid(p3, thickness, color);
	// drawCubeoid(p4, thickness, color);
	// drawCubeoid(p5, thickness, color);
	// drawCubeoid(p6, thickness, color);
	// drawCubeoid(p7, thickness, color);

	drawBeam(p0, p1, thickness, color);
	drawBeam(p0, p2, thickness, color);
	drawBeam(p3, p1, thickness, color);
	drawBeam(p3, p2, thickness, color);

	drawBeam(p4, p5, thickness, color);
	drawBeam(p4, p6, thickness, color);
	drawBeam(p7, p5, thickness, color);
	drawBeam(p7, p6, thickness, color);

	drawBeam(p0, p4, thickness, color);
	drawBeam(p1, p5, thickness, color);
	drawBeam(p2, p6, thickness, color);
	drawBeam(p3, p7, thickness, color);
}
