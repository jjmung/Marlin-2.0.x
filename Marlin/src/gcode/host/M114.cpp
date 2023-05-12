/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/stepper.h"

#if ENABLED(M114_DETAIL)

  #if HAS_L64XX
    #include "../../libs/L64XX/L64XX_Marlin.h"
    #define DEBUG_OUT ENABLED(L6470_CHITCHAT)
    #include "../../core/debug_out.h"
  #endif

  void report_all_axis_pos(const xyze_pos_t &pos, const uint8_t n=XYZE, const uint8_t precision=3) {
    char str[12];
    LOOP_L_N(a, n) {
      SERIAL_ECHOPGM_P((PGM_P)pgm_read_ptr(&SP_AXIS_LBL[a]));
      if (pos[a] >= 0) SERIAL_CHAR(' ');
      SERIAL_ECHO(dtostrf(pos[a], 1, precision, str));
    }
    SERIAL_EOL();
  }
  inline void report_linear_axis_pos(const xyze_pos_t &pos) { report_all_axis_pos(pos, XYZ); }

  void report_linear_axis_pos(const xyz_pos_t &pos, const uint8_t precision=3) {
    char str[12];
    LOOP_NUM_AXES(a) SERIAL_ECHOPGM_P((PGM_P)pgm_read_ptr(&SP_AXIS_LBL[a]), dtostrf(pos[a], 1, precision, str));
    SERIAL_EOL();
  }

  void report_current_position_detail() {
    // Position as sent by G-code
    SERIAL_ECHOPGM("\nLogical:");
    report_linear_axis_pos(current_position.asLogical());

    // Cartesian position in native machine space
    SERIAL_ECHOPGM("Raw:    ");
    report_linear_axis_pos(current_position);

    xyze_pos_t leveled = current_position;

    #if HAS_LEVELING
      // Current position with leveling applied
      SERIAL_ECHOPGM("Leveled:");
      planner.apply_leveling(leveled);
      report_linear_axis_pos(leveled);

      // Test planner un-leveling. This should match the Raw result.
      SERIAL_ECHOPGM("UnLevel:");
      xyze_pos_t unleveled = leveled;
      planner.unapply_leveling(unleveled);
      report_linear_axis_pos(unleveled);
    #endif

    #if IS_KINEMATIC
      // Kinematics applied to the leveled position
      SERIAL_ECHOPGM(TERN(IS_SCARA, "ScaraK: ", "DeltaK: "));
      inverse_kinematics(leveled);  // writes delta[]
      report_linear_axis_pos(delta);
    #endif

    planner.synchronize();

    #if HAS_L64XX
      char temp_buf[80];
      int32_t temp;
      //#define ABS_POS_SIGN_MASK 0b1111 1111 1110 0000 0000 0000 0000 0000
      #define ABS_POS_SIGN_MASK 0b11111111111000000000000000000000
      #define REPORT_ABSOLUTE_POS(Q) do{                            \
          L64xxManager.say_axis(Q, false);                          \
          temp = L6470_GETPARAM(L6470_ABS_POS,Q);                   \
          if (temp & ABS_POS_SIGN_MASK) temp |= ABS_POS_SIGN_MASK;  \
          sprintf_P(temp_buf, PSTR(":%8ld   "), temp);              \
          DEBUG_ECHO(temp_buf);                                     \
        }while(0)

      DEBUG_ECHOPGM("\nL6470:");
      #if AXIS_IS_L64XX(X)
        REPORT_ABSOLUTE_POS(X);
      #endif
      #if AXIS_IS_L64XX(X2)
        REPORT_ABSOLUTE_POS(X2);
      #endif
      #if AXIS_IS_L64XX(Y)
        REPORT_ABSOLUTE_POS(Y);
      #endif
      #if AXIS_IS_L64XX(Y2)
        REPORT_ABSOLUTE_POS(Y2);
      #endif
      #if AXIS_IS_L64XX(Z)
        REPORT_ABSOLUTE_POS(Z);
      #endif
      #if AXIS_IS_L64XX(Z2)
        REPORT_ABSOLUTE_POS(Z2);
      #endif
      #if AXIS_IS_L64XX(Z3)
        REPORT_ABSOLUTE_POS(Z3);
      #endif
      #if AXIS_IS_L64XX(Z4)
        REPORT_ABSOLUTE_POS(Z4);
      #endif
      #if AXIS_IS_L64XX(I)
        REPORT_ABSOLUTE_POS(I);
      #endif
      #if AXIS_IS_L64XX(J)
        REPORT_ABSOLUTE_POS(J);
      #endif
      #if AXIS_IS_L64XX(K)
        REPORT_ABSOLUTE_POS(K);
      #endif
      #if AXIS_IS_L64XX(E0)
        REPORT_ABSOLUTE_POS(E0);
      #endif
      #if AXIS_IS_L64XX(E1)
        REPORT_ABSOLUTE_POS(E1);
      #endif
      #if AXIS_IS_L64XX(E2)
        REPORT_ABSOLUTE_POS(E2);
      #endif
      #if AXIS_IS_L64XX(E3)
        REPORT_ABSOLUTE_POS(E3);
      #endif
      #if AXIS_IS_L64XX(E4)
        REPORT_ABSOLUTE_POS(E4);
      #endif
      #if AXIS_IS_L64XX(E5)
        REPORT_ABSOLUTE_POS(E5);
      #endif
      #if AXIS_IS_L64XX(E6)
        REPORT_ABSOLUTE_POS(E6);
      #endif
      #if AXIS_IS_L64XX(E7)
        REPORT_ABSOLUTE_POS(E7);
      #endif
      SERIAL_EOL();
    #endif // HAS_L64XX

    SERIAL_ECHOPGM("Stepper:");
    LOOP_LOGICAL_AXES(i) {
      SERIAL_ECHOPGM_P((PGM_P)pgm_read_ptr(&SP_AXIS_LBL[i]), stepper.position((AxisEnum)i));
    }
    SERIAL_EOL();

    #if IS_SCARA
      const xy_float_t deg = {
        planner.get_axis_position_degrees(A_AXIS),
        planner.get_axis_position_degrees(B_AXIS)
      };
      SERIAL_ECHOPGM("Degrees:");
      report_all_axis_pos(deg, 2);
    #endif

    SERIAL_ECHOPGM("FromStp:");
    get_cartesian_from_steppers();  // writes 'cartes' (with forward kinematics)
    xyze_pos_t from_steppers = LOGICAL_AXIS_ARRAY(
      planner.get_axis_position_mm(E_AXIS),
      cartes.x, cartes.y, cartes.z,
      planner.get_axis_position_mm(I_AXIS),
      planner.get_axis_position_mm(J_AXIS),
      planner.get_axis_position_mm(K_AXIS)
    );
    report_all_axis_pos(from_steppers);

    const xyze_float_t diff = from_steppers - leveled;
    SERIAL_ECHOPGM("Diff:   ");
    report_all_axis_pos(diff);

    TERN_(FULL_REPORT_TO_HOST_FEATURE, report_current_grblstate_moving());
  }

#endif // M114_DETAIL

/**
 * M114: Report the current position to host.
 *       Since steppers are moving, the count positions are
 *       projected by using planner calculations.
 *   D - Report more detail. This syncs the planner. (Requires M114_DETAIL)
 *   E - Report E stepper position (Requires M114_DETAIL)
 *   R - Report the realtime position instead of projected.
 */
void GcodeSuite::M114() {

  #if ENABLED(M114_DETAIL)
    if (parser.seen_test('D')) {
      #if DISABLED(M114_LEGACY)
        planner.synchronize();
      #endif
      report_current_position();
      report_current_position_detail();
      return;
    }
    #if HAS_EXTRUDERS
      if (parser.seen_test('E')) {
        SERIAL_ECHOLNPGM("Count E:", stepper.position(E_AXIS));
        return;
      }
    #endif
  #endif

  #if ENABLED(M114_REALTIME)
    if (parser.seen_test('R')) { report_real_position(); return; }
  #endif

  TERN_(M114_LEGACY, planner.synchronize());
  report_current_position_projected();

  TERN_(FULL_REPORT_TO_HOST_FEATURE, report_current_grblstate_moving());
}
