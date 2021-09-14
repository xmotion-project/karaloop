drawing_setup_file $PRO_DIRECTORY\text\prodetail.dtl
format_setup_file $PRO_DIRECTORY\text\prodetail.dtl
pro_unit_length unit_mm
pro_unit_mass unit_kilogram
template_designasm $PRO_DIRECTORY\templates\inlbs_asm_design.asm
template_new_ecadasm $PRO_DIRECTORY\templates\inlbs_ecad_asm.asm
template_drawing $PRO_DIRECTORY\templates\c_drawing.drw
template_sheetmetalpart $PRO_DIRECTORY\templates\inlbs_part_sheetmetal.prt
template_solidpart $PRO_DIRECTORY\templates\inlbs_part_solid.prt
template_boardpart $PRO_DIRECTORY\templates\inlbs_ecad_board.prt
todays_date_note_format %Mmm-%dd-%yy
tolerance_standard ansi
weld_ui_standard ansi

% -- PATH FOLDERS --
 search_path C:\Users\Quentus\switchdrive\xMotion\xMotion UNIGE - HEPIA\R&D\xMotion 3\xMotionP3\CAO\REALESED_VERSION\Headset band
 search_path C:\Users\Quentus\switchdrive\xMotion\xMotion UNIGE - HEPIA\R&D\xMotion 3\xMotionP3\CAO\REALESED_VERSION\Left hub
 search_path C:\Users\Quentus\switchdrive\xMotion\xMotion UNIGE - HEPIA\R&D\xMotion 3\xMotionP3\CAO\REALESED_VERSION\Right hub
 search_path C:\Users\Quentus\switchdrive\xMotion\xMotion UNIGE - HEPIA\R&D\xMotion 3\xMotionP3\CAO\REALESED_VERSION\Optical probe
 search_path C:\Users\Quentus\switchdrive\xMotion\xMotion UNIGE - HEPIA\R&D\xMotion 3\xMotionP3\CAO\REALESED_VERSION\Magnetic probe 
 