// Math3D header file

#include "Types.h"

#define PLANE_FRONT 1
#define PLANE_BACK 2
#define PLANE_ON 3

#define POLY_FRONT 1
#define POLY_BACK 2
#define POLY_SPANNING 3
#define POLY_CO_PLANAR 4

#define LINE_INTERSECTS 0
#define LINE_PARALLEL 1
#define LINE_CONTAINED 2
#define LINE_NO_INTERSECTION 3

#define VOID_INTERSECTION 1
#define VOID_BACKFACE_CULLING 2

HRESULT ComputeNormals(D3DVERTEX *pVertices, WORD *pIndices,
                       DWORD dwNumVertices, DWORD dwNumIndices);
Vector3D GetPolygonNormal(Polygon3 *pPolygon);
Plane GetPolygonPlane(Polygon3 *pPolygon);
int GetSideOfPlane(Vector3D *pvPoint, Plane *pPlane);
int GetSideOfPolygon(Vector3D *pvPoint, Polygon3 *pPolygon);
int GetLinePlaneIntersection(Vector3D &vIntersect, CLineBase *pLine, Plane *pPlane, DWORD dwFlags);
bool GetPointInPolygon(Vector3D *vPoint, Polygon3 *pPolygon);
bool GetLinePolygonIntersection(Vector3D &vIntersect, CLineBase *pLine, Polygon3 *pPolygon);
Vector3D Normalize(Vector3D *v);

inline FPOINT GetDistance2D(FPOINT x0, FPOINT y0, FPOINT x1, FPOINT y1);
inline FPOINT GetDistance3D(FPOINT x0, FPOINT y0, FPOINT z0, FPOINT x1, FPOINT y1, FPOINT z1);
inline FPOINT GetDistance3D2(Point3D p0, Point3D p1);
