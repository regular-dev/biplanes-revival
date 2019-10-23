#ifndef ZEPPELIN_H
#define ZEPPELIN_H


class Zeppelin
{
  float x;
  float y;
  bool ascent;

public:
  Zeppelin();

  void Update();
  void UpdateCoordinates();
  void Draw();
  void DrawScore();

  void Respawn();
};


#endif //ZEPPELIN_H
