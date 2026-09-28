import argparse
import math
import re
import struct
from pathlib import Path


GRAPHIC_PATTERN = re.compile(
    r"(?P<command>\\includegraphics\s*)"
    r"(?:\[(?P<options>[^\]]*)\])?"
    r"(?P<before_path>\s*)\{(?P<path>[^{}]+)\}"
)
WIDTH_PATTERN = re.compile(r"^width\s*=", re.IGNORECASE)
SCALE_PATTERN = re.compile(r"^scale\s*=", re.IGNORECASE)
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
FULL_WIDTH_DIMENSION_THRESHOLD = 900


def calculate_width_fraction(pixel_width, pixel_height, max_pixel_area, min_width=0.55):
    """Return full width for large images, otherwise scale by pixel area."""
    if max(pixel_width, pixel_height) > FULL_WIDTH_DIMENSION_THRESHOLD:
        return 1.0

    image_area = pixel_width * pixel_height
    return max(min_width, math.sqrt(image_area / max_pixel_area))


def read_png_dimensions(path):
    with path.open("rb") as image_file:
        header = image_file.read(24)
    if len(header) < 24 or header[:8] != PNG_SIGNATURE:
        raise ValueError(f"Expected a PNG image: {path}")
    return struct.unpack(">II", header[16:24])


def resolve_image(doc_dir, tex_path, image_reference):
    image_path = Path(image_reference)
    candidates = [tex_path.parent / image_path]
    if not image_path.is_absolute():
        candidates.append(doc_dir / "images" / image_path.name)
    if image_path.suffix == "":
        candidates = [candidate.with_suffix(".png") for candidate in candidates]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    raise FileNotFoundError(f"Cannot find image {image_reference!r} referenced by {tex_path}")


def format_width(fraction):
    if math.isclose(fraction, 1.0):
        return r"\linewidth"
    value = f"{fraction:.3f}".rstrip("0").rstrip(".")
    return rf"{value}\linewidth"


def replace_widths(text, tex_path, image_widths):
    def replace(match):
        image_path = resolve_image(tex_path.parents[1], tex_path, match.group("path"))
        width = format_width(image_widths[image_path])
        options = [option.strip() for option in (match.group("options") or "").split(",")]
        options = [
            option
            for option in options
            if option and not WIDTH_PATTERN.match(option) and not SCALE_PATTERN.match(option)
        ]
        options.insert(0, f"width={width}")
        return (
            match.group("command")
            + "["
            + ",".join(options)
            + "]"
            + match.group("before_path")
            + "{"
            + match.group("path")
            + "}"
        )

    return GRAPHIC_PATTERN.subn(replace, text)


def main():
    parser = argparse.ArgumentParser(
        description="Set diagram widths in proportion to PNG resolution."
    )
    parser.add_argument(
        "--min-width",
        type=float,
        default=0.55,
        help="Minimum width as a fraction of linewidth (default: 0.55).",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print calculated widths without changing TeX files.",
    )
    args = parser.parse_args()
    if not 0 < args.min_width <= 1:
        parser.error("--min-width must be greater than 0 and at most 1")

    doc_dir = Path(__file__).resolve().parents[1] / "doc"
    tex_files = sorted(doc_dir.rglob("*.tex"))
    references = {}
    for tex_path in tex_files:
        text = tex_path.read_text()
        for match in GRAPHIC_PATTERN.finditer(text):
            image_path = resolve_image(doc_dir, tex_path, match.group("path"))
            references.setdefault(image_path, read_png_dimensions(image_path))

    if not references:
        print("No PNG graphics found.")
        return

    max_pixel_area = max(width * height for width, height in references.values())
    image_widths = {
        path: calculate_width_fraction(width, height, max_pixel_area, args.min_width)
        for path, (width, height) in references.items()
    }

    max_image = max(references, key=lambda path: references[path][0] * references[path][1])
    max_width, max_height = references[max_image]
    print(
        f"Largest image: {max_image.name} ({max_width}x{max_height}); "
        r"width=\linewidth"
    )
    for path in sorted(references):
        width, height = references[path]
        print(f"{path.name}: {width}x{height} -> {format_width(image_widths[path])}")

    total_graphics = 0
    updated_files = 0
    for tex_path in tex_files:
        original = tex_path.read_text()
        updated, count = replace_widths(original, tex_path, image_widths)
        total_graphics += count
        if updated != original:
            updated_files += 1
            if not args.dry_run:
                tex_path.write_text(updated)

    action = "Would update" if args.dry_run else "Updated"
    print(f"{action} {total_graphics} graphics in {updated_files} TeX files.")


if __name__ == "__main__":
    main()