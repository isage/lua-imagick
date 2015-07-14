luaunit = require('luaunit')

TestGM = {}
TestIM = {}

    function TestGM:setUp() 
      self.ima = require("gimagick.gm")
    end

    function TestIM:setUp() 
      self.ima = require("gimagick.im")
    end

    function TestGM:test00_Api()
        luaunit.assertEquals( self.ima.api(), 'gm' )
    end

    function TestIM:test00_Api()
        luaunit.assertEquals( self.ima.api(), 'im' )
    end

    function TestGM:test01_Open()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertIsUserdata(img)
    end

    function TestIM:test01_Open()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertIsUserdata(img)
    end

    function TestGM:test02_NotOpen()
        local img, err = self.ima.open("not.exists")
        luaunit.assertNil(img)
        luaunit.assertEquals(err, "Unable to open file (not.exists)")
    end

    function TestIM:test02_NotOpen()
        local img, err = self.ima.open("not.exists")
        luaunit.assertNil(img)
        luaunit.assertStrContains(err, "unable to open image `not.exists'")
    end

    function TestGM:test03_ToString()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(tostring(img), "JPEG(768x576)")
    end

    function TestIM:test03_ToString()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(tostring(img), "JPEG(768x576)")
    end

    function TestGM:test04_GetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
    end

    function TestIM:test04_GetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
    end

    function TestGM:test05_SetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
        img:set_format("PNG")
        luaunit.assertEquals(img:get_format(), "PNG")
    end

    function TestIM:test05_SetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
        img:set_format("PNG")
        luaunit.assertEquals(img:get_format(), "PNG")
    end

    function TestGM:test06_Write()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write("out.png")
        luaunit.assertTrue(suc)
    end

    function TestIM:test06_Write()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write("out.png")
        luaunit.assertTrue(suc)
    end

    function TestGM:test07_WriteAll()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write_all("out.gif")
        luaunit.assertTrue(suc)
    end

    function TestIM:test07_WriteAll()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write_all("out.gif")
        luaunit.assertTrue(suc)
    end

os.exit( luaunit.LuaUnit.run() )

