local assert = require("luassert")
local xlib = require("xlib")

describe("xlib", function()
    local display = xlib.XOpenDisplay()

    describe("XInternAtom", function()
        it("returns the same Atom for a name", function()
            local name = "lua-xlib.no_duplicates"
            local first = xlib.XInternAtom(display, name)
            local second = xlib.XInternAtom(display, name)
            assert.is_equal(first, second)
        end)

        it("returns `None` on non-existent Atom", function()
            assert.is_equal(0, xlib.XInternAtom(display, "lua-xlib.does_not_exist", true))
        end)
    end)

    describe("XInternAtoms", function()
        it("returns all atoms", function()
            local first_name = "lua-xlib.list_one"
            local second_name = "lua-xlib.list_two"

            local first = xlib.XInternAtom(display, first_name)
            local second = xlib.XInternAtom(display, second_name)

            local status, list = xlib.XInternAtoms(display, { first_name, second_name })
            assert.is_not_equal(0, status)
            assert.is_same({ first, second }, list)
        end)
    end)

    describe("XGetAtomName", function()
        it("returns the name the Atom was created with", function()
            local name = "lua-xlib.a_unique_name"
            local atom = xlib.XInternAtom(display, name)

            assert.is_equal(name, xlib.XGetAtomName(display, atom))
        end)
    end)

    describe("XGetAtomNames", function()
        it("returns all names", function()
            local first_name = "lua-xlib.list_one"
            local second_name = "lua-xlib.list_two"

            local first = xlib.XInternAtom(display, first_name)
            local second = xlib.XInternAtom(display, second_name)

            local status, list = xlib.XGetAtomNames(display, { first, second })
            assert.is_not_equal(0, status)
            assert.is_same({ first_name, second_name }, list)
        end)
    end)
end)
