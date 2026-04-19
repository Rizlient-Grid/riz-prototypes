#!/usr/bin/env python3
import argparse
import csv
import datetime as dt
import re
import sys
import time

import serial


DEFAULT_COMMAND = "cmd_pwm_measure2_read"
DEFAULT_BAUDRATE = 115200
DEFAULT_INTERVAL_SECONDS = 1.0
DEFAULT_SAMPLE_COUNT = 1000
DEFAULT_TIMEOUT_SECONDS = 1.0
RESPONSE_PATTERN = re.compile(
    r"^\s*>\s*cmd_pwm_measure2_read:\s*(\d+)Hz\s+(\d+(?:\.\d+)?)%\s*$"
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Poll PWM measurement data over serial and save results to CSV."
    )
    parser.add_argument("--port", required=True, help="Serial port, e.g. /dev/tty.usbmodem1101")
    parser.add_argument("--baudrate", type=int, default=DEFAULT_BAUDRATE, help="Serial baudrate")
    parser.add_argument(
        "--interval",
        type=float,
        default=DEFAULT_INTERVAL_SECONDS,
        help="Sampling interval in seconds",
    )
    parser.add_argument(
        "--samples",
        type=int,
        default=DEFAULT_SAMPLE_COUNT,
        help="Number of valid samples to collect",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT_SECONDS,
        help="Serial read timeout in seconds",
    )
    parser.add_argument(
        "--output",
        default="pwm_measurements.csv",
        help="Output CSV path",
    )
    parser.add_argument(
        "--command",
        default=DEFAULT_COMMAND,
        help="Command sent to the device",
    )
    return parser.parse_args()


def read_measurement(ser: serial.Serial, command: str, timeout_seconds: float) -> tuple[int, float, str]:
    deadline = time.monotonic() + timeout_seconds
    ser.reset_input_buffer()
    ser.write((command + "\r\n").encode("ascii"))
    ser.flush()

    while time.monotonic() < deadline:
        raw = ser.readline()
        if not raw:
            continue

        line = raw.decode("utf-8", errors="replace").strip()
        if not line or "TRACE:" in line:
            continue

        match = RESPONSE_PATTERN.match(line)
        if match:
            freq_hz = int(match.group(1))
            duty_percent = float(match.group(2))
            return freq_hz, duty_percent, line

    raise TimeoutError(f"No valid measurement response received within {timeout_seconds:.1f}s")


def main() -> int:
    args = parse_args()
    start_time = time.monotonic()
    next_sample_time = start_time

    with serial.Serial(args.port, args.baudrate, timeout=args.timeout) as ser, open(
        args.output, "w", newline="", encoding="utf-8"
    ) as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(
            [
                "sample_index",
                "timestamp_iso",
                "elapsed_seconds",
                "freq_hz",
                "duty_percent",
                "raw_response",
            ]
        )

        sample_index = 0
        while sample_index < args.samples:
            now = time.monotonic()
            if now < next_sample_time:
                time.sleep(next_sample_time - now)

            timestamp = dt.datetime.now(dt.timezone.utc).astimezone()
            try:
                freq_hz, duty_percent, raw_response = read_measurement(
                    ser, args.command, args.timeout
                )
            except TimeoutError as exc:
                print(
                    f"[WARN] sample {sample_index + 1}: {exc}",
                    file=sys.stderr,
                )
                next_sample_time += args.interval
                continue

            elapsed_seconds = time.monotonic() - start_time
            sample_index += 1

            writer.writerow(
                [
                    sample_index,
                    timestamp.isoformat(),
                    f"{elapsed_seconds:.3f}",
                    freq_hz,
                    f"{duty_percent:.1f}",
                    raw_response,
                ]
            )
            csvfile.flush()

            print(
                f"[{sample_index:04d}/{args.samples}] "
                f"freq={freq_hz}Hz duty={duty_percent:.1f}%"
            )

            next_sample_time += args.interval

    print(f"Saved {args.samples} samples to {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
