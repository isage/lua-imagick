luaunit = require('luaunit')

TestGM = {}
TestIM = {}

    function TestGM:setUp() 
      self.ima = require("gimagick.gm")
    end

    function TestIM:setUp() 
      self.ima = require("gimagick.im")
    end

    function TestGM:testApi()
        luaunit.assertEquals( self.ima.api(), 'gm' )
    end

    function TestIM:testApi()
        luaunit.assertEquals( self.ima.api(), 'im' )
    end

    function TestGM:testOpen()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertIsUserdata(img)
    end

    function TestIM:testOpen()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertIsUserdata(img)
    end

    function TestGM:testNotOpen()
        local img, err = self.ima.open("not.exists")
        luaunit.assertNil(img)
        luaunit.assertEquals(err, "Unable to open file (not.exists)")
    end

    function TestIM:testNotOpen()
        local img, err = self.ima.open("not.exists")
        luaunit.assertNil(img)
        luaunit.assertStrContains(err, "unable to open image `not.exists'")
    end

    function TestGM:testToString()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(tostring(img), "JPEG(400x600)")
    end

    function TestIM:testToString()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(tostring(img), "JPEG(400x600)")
    end

    function TestGM:testGetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
    end

    function TestIM:testGetFormat()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        luaunit.assertEquals(img:get_format(), "JPEG")
    end

    function TestGM:testWrite()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write("out.png")
        luaunit.assertTrue(suc)
    end

    function TestIM:testWrite()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write("out.png")
        luaunit.assertTrue(suc)
    end

    function TestGM:testWriteAll()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write_all("out.gif")
        luaunit.assertTrue(suc)
    end

    function TestIM:testWriteAll()
        local img, err = self.ima.open("input.jpg")
        luaunit.assertNotNil(img)
        local suc, err = img:write_all("out.gif")
        luaunit.assertTrue(suc)
    end

os.exit( luaunit.LuaUnit.run() )

