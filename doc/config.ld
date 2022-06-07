project = 'lua-xlib'
title = 'lua-xlib'
description = "Lua bindings for XLib and XRandR"

topics = {
    'doc/pages/randr.md',
}

template = true

format = 'discount'

pretty = 'lua'
-- prettify_files = 'show'
backtick_references = false
wrap = true
no_space_before_args = true

custom_tags = {
    {
        "async",
        hidden = true,
    },
    {
        "repository",
        hidden = true,
    },
    {
        "package",
        hidden = true,
    }
}
