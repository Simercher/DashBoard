slint::include_modules!();

pub fn create_main_window() -> MainWindow {
    let window = MainWindow::new().unwrap();
    window.set_battery_voltage(12.5); // 初始化電壓值
    window
}
