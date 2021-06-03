#include "segment_display.h"
#include "timing.h"


constexpr int8_t INVALID_DIFF = 0;
constexpr uint8_t INVALID_BYTE_I = 0xFF;

struct AnimationPath{
  int8_t diff;
  uint8_t byte_i;
  AnimationPath(int8_t d = INVALID_DIFF, uint8_t b_i = INVALID_BYTE_I) : diff(d), byte_i(b_i)
  {}
  bool valid() const{
    return diff != INVALID_DIFF;
  }
};


const AnimationPath paths[6][4]{
  {AnimationPath( 1, 0), AnimationPath( 5, 5), AnimationPath(-5), AnimationPath()},
  {AnimationPath( 1, 1), AnimationPath(-1, 0), AnimationPath( 5), AnimationPath()},
  {AnimationPath( 1, 2), AnimationPath(-1, 1), AnimationPath(3, 6), AnimationPath(9)},
  {AnimationPath( 1, 3), AnimationPath(-1, 2), AnimationPath( 7), AnimationPath()},
  {AnimationPath( 1, 4), AnimationPath(-1, 3), AnimationPath(-7), AnimationPath()},
  {AnimationPath(-5, 5), AnimationPath(-1, 4), AnimationPath(-3, 6), AnimationPath(-9)}
};

constexpr uint8_t MAX_POS = 24;
constexpr uint8_t SEGMENT_NONE = 0xFF;
template<int length>
class AnimatedSnake{
  uint8_t last_move_i;
  uint8_t segments_on[length];
  uint8_t last_pos;
public:
  AnimatedSnake() : last_move_i(0){
    last_pos = random(MAX_POS);
    for (int i = 0; i < length; i++) segments_on[i] = SEGMENT_NONE;
  }
  void move(uint8_t* state){
    const AnimationPath* ps = paths[last_pos % 6];
    AnimationPath p;
    while (true){
      p = ps[random(4)];
      if (p.valid() && last_pos + p.diff < MAX_POS && last_pos >= -p.diff){
        break;
      }
    }
    last_pos += p.diff;
    last_move_i = (last_move_i + 1) % length;
    uint8_t* seg = &segments_on[last_move_i];
    turnSegmentOff(state, *seg);
    *seg = (p.byte_i == INVALID_BYTE_I) ? SEGMENT_NONE : last_pos / 6 * 8 + p.byte_i;
    turnSegmentOn(state, *seg);
  }
private:
  inline void turnSegmentOff(uint8_t* state, uint8_t seg){
    if (seg == SEGMENT_NONE) return;
    state[seg / 8] |= 1 << (seg % 8);
  }
  inline void turnSegmentOn(uint8_t* state, uint8_t seg){
    if (seg == SEGMENT_NONE) return;
    state[seg / 8] &= ~(1 << (seg % 8));
  }
};




template<int snake_count, int snake_length>
class AnimatedSegmentDisplay : public SegmentDisplay{
  AnimatedSnake<snake_length> snakes[snake_count];
  Timer<> timer;
public:
  AnimatedSegmentDisplay(uint16_t move_period) : timer(move_period)
  {}
  void update(){
    if (timer.update()) for (int i = 0; i < snake_count; i++) snakes[i].move(seg_state);
    SegmentDisplay::update();
  }
};