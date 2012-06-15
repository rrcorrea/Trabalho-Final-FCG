#ifndef MODELPORSCHE_H
#define MODELPORSCHE_H



#include <stdio.h>
#include <string.h>
#include "3DObject.h"
#include "Sphere.h"
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ModelPorsche {

//-----------------------------------------------------------------------------
protected:

  /// object
  C3DObject *m_poObject;

  ///
  vector<CSphere *> m_vpoSpheres;

  /// position
  float m_afPosition[3];

  /// orientation
  float m_fAngleY;
  float m_fAngleZ;
  float m_fAngleX;


  /// control when the model was hit by a shot
  bool m_bWasHit;

  /// total number of Al's instantiated
  static int m_nTotalPors;

//-----------------------------------------------------------------------------
public:

  /// constructor
  ModelPorsche();

  /// destructor
  ~ModelPorsche();

  /// initialize the Al object
  bool Init();

  /// draw the model
  void Draw(bool bnDrawSpheres = false);

  /// check collision
  float CheckCollisionRay(float *pfPosition, float *pfDirection);

  /// rotate the model
  void RotateY(float fAngle);
  void RotateZ(float fAngle);
  void RotateX(float fAngle);


  /// translate the model
  void Translate(float fX, float fY, float fZ);

  /// return the total number of objects still "alive"
  static int GetTotalCount() { return m_nTotalPors; }

};

#endif // MODELPORSCHE_H
