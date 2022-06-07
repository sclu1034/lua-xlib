# X RandR

> A collection of links and snippets about RandR, XRandR and using those.

### Links

- [Protocol Specification](https://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt)
- [XRandR.h](https://cgit.freedesktop.org/xorg/lib/libXrandr/tree/include/X11/extensions/Xrandr.h)
- [X terminology overview](https://www.x.org/wiki/Development/Documentation/HowVideoCardsWork)

### Snippets

Certain function parameters and struct members are documented as "<some_value> or None", however the "None" is
neither linked nor otherwise documented.
It is defined in `X.h` as simply `0`:

```c
#ifndef None
#define None                 0L	/* universal null resource or null atom */
#endif
```

----
