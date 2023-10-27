use chrono::{DateTime, Local};
use std::time::{Duration, SystemTime};

fn main() {
    let port_name = std::env::var("CLOCK_SERIAL_PORT").unwrap_or("/dev/ttyUSB0".into());
    let mut port = serialport::new(&port_name, 9600)
        .flow_control(serialport::FlowControl::None)
        .open()
        .expect("Could not open port");

    loop {
        let local: DateTime<Local> = DateTime::from(SystemTime::now());
        let local = local.time().format("%H%M%S").to_string();
        writeln!(port, "{}", local).unwrap();
        std::thread::sleep(Duration::from_millis(50));
    }
}
