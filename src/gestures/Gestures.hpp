#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <wayfire/touch/touch.hpp>
#include <wlr/types/wlr_touch.h>

// Swipe params
constexpr static int EDGE_SWIPE_THRESHOLD              = 10;
constexpr static double MIN_SWIPE_DISTANCE             = 30;
constexpr static double MAX_SWIPE_DISTANCE             = 450;
constexpr static double SWIPE_INCORRECT_DRAG_TOLERANCE = 150;

// Pinch params
constexpr static double PINCH_INCORRECT_DRAG_TOLERANCE = 200;
constexpr static double PINCH_THRESHOLD                = 1.5;

// Hold params
constexpr static double HOLD_INCORRECT_DRAG_TOLERANCE = 100;

// General
constexpr static double GESTURE_INITIAL_TOLERANCE = 40;
constexpr static uint32_t GESTURE_BASE_DURATION   = 400;

enum eTouchGestureType {
    // Invalid Gesture
    GESTURE_TYPE_NONE,
    GESTURE_TYPE_SWIPE,
    GESTURE_TYPE_EDGE_SWIPE,
    // GESTURE_TYPE_PINCH,
};

enum eTouchGestureDirection {
    /* Swipe-specific */
    GESTURE_DIRECTION_LEFT  = (1 << 0),
    GESTURE_DIRECTION_RIGHT = (1 << 1),
    GESTURE_DIRECTION_UP    = (1 << 2),
    GESTURE_DIRECTION_DOWN  = (1 << 3),
    /* Pinch-specific */
    // GESTURE_DIRECTION_IN = (1 << 4),
    // GESTURE_DIRECTION_OUT = (1 << 5),
};

// can be one of @eTouchGestureDirection or a combination of them
using gestureDirection = uint32_t;

// swipe and with multiple fingers and directions
class CMultiAction : public wf::touch::gesture_action_t {
  public:
    CMultiAction(double threshold) : threshold(threshold){};
    // bool pinch;
    // bool last_pinch_was_pinch_in = false;
    double threshold;

    gestureDirection target_direction = 0;
    int finger_count                  = 0;

    wf::touch::action_status_t
    update_state(const wf::touch::gesture_state_t& state,
                 const wf::touch::gesture_event_t& event) override;

    void reset(uint32_t time) override {
        gesture_action_t::reset(time);
        target_direction = 0;
    };
};

using edge_swipe_callback = std::function<void(CMultiAction*)>;

std::unique_ptr<wf::touch::gesture_t>
newWorkspaceSwipeStartGesture(const double sensitivity, const int fingers,
                              wf::touch::gesture_callback_t completed_cb,
                              wf::touch::gesture_callback_t cancel_cb);

std::unique_ptr<wf::touch::gesture_t>
newEdgeSwipeGesture(const double sensitivity, edge_swipe_callback completed_cb,
                    edge_swipe_callback cancel_cb);

/*
 * Interface; there's only @CGestures and the mock gesture manager for testing
 * that implements this
 */
class IGestureManager {
  public:
    virtual ~IGestureManager() {}
    virtual bool onTouchDown(wlr_touch_down_event*);
    virtual bool onTouchUp(wlr_touch_up_event*);
    virtual bool onTouchMove(wlr_touch_motion_event*);

    void addTouchGesture(std::unique_ptr<wf::touch::gesture_t> gesture);

  protected:
    std::vector<std::unique_ptr<wf::touch::gesture_t>> m_vGestures;
    wf::touch::gesture_state_t m_sGestureState;

  private:
    void updateGestures(const wf::touch::gesture_event_t&);
};
