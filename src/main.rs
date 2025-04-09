mod ui;
use slint::ComponentHandle;

fn main() -> Result<(), slint::PlatformError> {
    let window = ui::create_main_window();
    window.run()
}
