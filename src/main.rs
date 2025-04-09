use slint::ComponentHandle; // 引入 ComponentHandle 特徵
mod ui;

fn main() -> Result<(), slint::PlatformError> {
    let _window = ui::MainWindow::new()?;

    _window.on_clicked({
        let window_handle = _window.as_weak();
        move || {
            if let Some(_window) = window_handle.upgrade() {
                // 定義按鈕點擊時的行為，例如：
                println!("按鈕被點擊了！");
            }
        }
    });

    _window.run()
}
