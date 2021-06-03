#include "segment_display.h"
#include "timing.h"


constexpr int8_t INVALID_DIFF = 0;
constexpr uint8_t INVALID_BYTE_I = 0xFF;


/**
 * AnimationPath
 *  - Holds one possible path for the display snake animation
 *  - 
 */
struct AnimationPath{
  int8_t diff;  //difference of index of the field this path leads to
  uint8_t byte_i; //index of byte in display segment this path corresponds to
  AnimationPath(int8_t d = INVALID_DIFF, uint8_t b_i = INVALID_BYTE_I) : diff(d), byte_i(b_i)
  {}
  bool valid() const{
    return diff != INVALID_DIFF;
  }
};



/**
 * The animation is one of a snake, going over given paths on the display
 * All paths on one segment of the whole display - first index identifies, which vertex the path corresponds to, vertices on one segment are organized as follows
 *  ONE DIGIT     VERTEX INDICES
 *  -----         0---1 - - - 6---7 - - 
 *  |   |         |   |       |   |
 *  -----         5---2 - - - 11--8 - -
 *  |   |         |   |       |   |
 *  -----.        4---3.- - - 10--9.- -
 * Last path in a series leads to another digit - e.g. from vertex index one, we can go to the next digit. 
 * The following array describes all paths on one digit. Because the AnimationClass contains a relative position to current index, it is the same for all digits, despite them having different indices.
 */ 

const AnimationPath paths[6][4]{
  {AnimationPath( 1, 0), AnimationPath( 5, 5), AnimationPath(-5), AnimationPath()},
  {AnimationPath( 1, 1), AnimationPath(-1, 0), AnimationPath( 5), AnimationPath()},
  {AnimationPath( 1, 2), AnimationPath(-1, 1), AnimationPath(3, 6), AnimationPath(9)},
  {AnimationPath( 1, 3), AnimationPath(-1, 2), AnimationPath( 7), AnimationPath()},
  {AnimationPath( 1, 4), AnimationPath(-1, 3), AnimationPath(-7), AnimationPath()},
  {AnimationPath(-5, 5), AnimationPath(-1, 4), AnimationPath(-3, 6), AnimationPath(-9)}
};




constexpr uint8_t DISPLAY_DIGIT_COUNT = 4;
constexpr uint8_t MAX_POS = 6 * DISPLAY_DIGIT_COUNT;
constexpr uint8_t SEGMENT_NONE = 0xFF;  //snake segment not used yet

//Holds one display snake
template<int length>
class AnimatedSnake{
  uint8_t last_move_i;  //index of last move into segments array - goes from 0 to length - 1, overwriting 
  //list of segments this snake uses. (segment / 8) indicates digit index on the whole display, (segment % 8) indicates byte index in current segment
  uint8_t segments_on[length];
  uint8_t last_pos;     //current position of snake head (index of vertex)
public:
  AnimatedSnake() : last_move_i(0){
    last_pos = random(MAX_POS);
    for (int i = 0; i < length; i++) segments_on[i] = SEGMENT_NONE;
  }
  void move(uint8_t* state){
    //select all paths available from curent position
    const AnimationPath* ps = paths[last_pos % 6];
    AnimationPath p;
    //select a random valid path
    while (true){
      p = ps[random(4)];
      //path exists & does not lead outside of display
      if (p.valid() && last_pos + p.diff < MAX_POS && last_pos >= -p.diff){
        break;
      }
    }
    //compute new position
    last_pos += p.diff;
    last_move_i = (last_move_i + 1) % length;
    uint8_t* seg = &segments_on[last_move_i];
    //turn of the last segment of the snake
    turnSegmentOff(state, *seg);
    //if digit is invalid, segment will be off. else:
    //last_pos / 6 -> digit index, * 8 = offset of digit in bytes, + byte_i = index of byte to turn on.
    *seg = (p.byte_i == INVALID_BYTE_I) ? SEGMENT_NONE : last_pos / 6 * 8 + p.byte_i;
    //turn on the first segment of the snake
  }
  //draw all segments. This isn't done in update similar to turning off because when there are multiple snakes, they can easily turn off parts of each other (when overlapping)
  void draw(uint8_t* state){
    for (uint8_t s : segments_on){
      turnSegmentOn(state, s);
    }
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
  void init(){
    SegmentDisplay::init();
    timer.init();
  }
  void update(){
    if (timer.update()){
      //move all snakes
      for (int i = 0; i < snake_count; i++) snakes[i].move(seg_state);
      //draw them to the display
      for (int i = 0; i < snake_count; i++) snakes[i].draw(seg_state);
    }
    SegmentDisplay::update();
  }
};