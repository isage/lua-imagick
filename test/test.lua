luaunit = require('luaunit')

TestIM = {}

    function TestIM:setUp()
      self.ima = require("imagick")
    end

    function TestIM:test01_Open()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertIsUserdata(img)
    end

    function TestIM:test02_NotOpen()
        local img, err = self.ima.open("not.exists")
        luaunit.assertNil(img)
        luaunit.assertStrContains(err, "unable to open image `not.exists'")
    end

    function TestIM:test03_ToString()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(tostring(img), "JPEG(768x576)")
    end

    function TestIM:test04_GetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
    end

    function TestIM:test05_SetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
        img:set_format("PNG")
        luaunit.assertEquals(img:get_format(), "PNG")
    end

    function TestIM:test06_Write()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write("out.png")
        luaunit.assertTrue(suc)
    end

    function TestIM:test07_WriteAll()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write_all("out.gif")
        luaunit.assertTrue(suc)
    end

    function TestIM:test08_Blob()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local blob, len = img:blob()
        luaunit.assertStrContains(blob, "JFIF")
    end

    function TestIM:test09_Quality()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local quality = img:get_quality()
        luaunit.assertEquals(quality, 92)
        img:set_quality(90)
        quality = img:get_quality()
        luaunit.assertEquals(quality, 90)
    end

    function TestIM:test10_Gravity()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local gravity = img:get_gravity()
        luaunit.assertEquals(gravity, self.ima.gravity['ForgetGravity'])
        img:set_gravity(self.ima.gravity['WestGravity'])
        gravity = img:get_gravity()
        luaunit.assertEquals(gravity, self.ima.gravity['WestGravity'])
    end

    function TestIM:test11_Colorspace()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local colorspace = img:get_colorspace()
        luaunit.assertEquals(colorspace, self.ima.colorspace['sRGBColorspace'])
    end

    function TestIM:test12_Alphachannel()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertFalse(img:has_alphachannel())

        img, err = self.ima.open("input.png")
        luaunit.assertNotNil(img)
        luaunit.assertTrue(img:has_alphachannel())
    end

    function TestIM:test13_GetICCProfile()
        local img, err = self.ima.open("input.cmyk.jpg")
        luaunit.assertNotNil(img)
        local blob, len = img:get_icc_profile()
        luaunit.assertEquals(len, 1829077)
    end

    function TestIM:test14_HasICCProfile()
        local img, err = self.ima.open("input.cmyk.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertTrue(img:has_icc_profile())
    end

    function TestIM:test15_GetBGColor()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local color = img:get_bg_color()
        luaunit.assertEquals(color, "1,1,1")
    end

    function TestIM:test16_Read()
        local img = self.ima.new()
        luaunit.assertIsUserdata(img)
        local suc = img:read("input.jpg")
        luaunit.assertTrue(suc)
    end

    function TestIM:test17_SetResolution()
        local img = self.ima.new()
        img:set_resolution(72, 72)
        local suc = img:read("input.svg")
        luaunit.assertTrue(suc)
        local width = img:width()
        local height = img:height()

        img = self.ima.new()
        img:set_resolution(144, 144)
        suc = img:read("input.svg")
        luaunit.assertTrue(suc)
        luaunit.assertEquals(img:width(), width * 2)
        luaunit.assertEquals(img:height(), height * 2)
    end

os.exit( luaunit.LuaUnit.run() )
