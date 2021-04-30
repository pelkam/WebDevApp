'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files){
    console.log("Invalid file")
    return res.redirect('/');
  }
  let uploadFile = req.files.uploadFile;
  if(path.extname(uploadFile.name).localeCompare(".gpx")!=0){
    console.log("Invalid file")
    return res.redirect('/');
  }
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }
    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.query.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.query.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 
app.get('/upload/:filename',function(req , res){
  let filenames = '';
  fs.readdirSync('uploads/').forEach(file => {
    filenames += file+'|';
  });
  res.send({
    files: filenames
  })
});
app.get('/gpxJson',function(req,res){
  var string="";
  //For mac lib parser must end in .dylib
  //For school server lib parser must end in .so
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'uploadGPXFile': [ 'string',['string']]
  });
  string = gpx.uploadGPXFile(path.join(__dirname+"/uploads/"+req.query.file));
  if(string.localeCompare("[]")==0){
    string = "";
  }
  res.send({
    gpxString: string
  })
});
app.get('/gpxRoute',function(req,res){
  var string ="";
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'getViewPanelRoute': [ 'string',['string']]
  });
  string = gpx.getViewPanelRoute(path.join(__dirname+"/uploads/"+req.query.file));
  if(string.localeCompare("[]")==0){
    string = "";
  }
  res.send({
    routeString: string
  })
});
app.get('/gpxTrack',function(req,res){
  var string ="";
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'getViewPanelTrack': [ 'string',['string']]
  });
  string = gpx.getViewPanelTrack(path.join(__dirname+"/uploads/"+req.query.file));
  if(string.localeCompare("[]")==0){
    string = "";
  }
  res.send({
    trackString: string
  })
});
app.get('/gpxWaypoint',function(req,res){
  var string ="";
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'getWaypointInfo': [ 'string',['string','int']]
  });
  string = gpx.getWaypointInfo(path.join(__dirname+"/uploads/"+req.query.file),req.query.rtId);
  if(string.localeCompare("[]")==0){
    string = "";
  }
  res.send({
    wptString: string
  })
});
app.get('/gpxData',function(req,res){
  var string ="";
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'getDataList': [ 'string',['string','string','string']]
  });
  string = gpx.getDataList(path.join(__dirname+"/uploads/"+req.query.file),req.query.name,req.query.idNum);
  res.send({
    dataString: string
  })
});
app.get('/rename',function(req,res){
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'changeName': [ 'int',['string','string','string','string']]
  });
  var int = gpx.changeName(path.join(__dirname+"/uploads/"+req.query.file),req.query.name,req.query.idNum,req.query.rename)
  res.send({
    stat: int
  })
});
app.get('/create',function(req,res){
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'createGPXfile': [ 'int',['string','string']]
  });
  var i = gpx.createGPXfile(path.join(__dirname+"/uploads/"+req.query.file),req.query.json)
  res.send({
    data: i,
    file:req.query.file
  })
});
app.get('/createRte',function(req,res){
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'createRte': [ 'int',['string','string']]
  });
  var i = gpx.createRte(path.join(__dirname+"/uploads/"+req.query.file),req.query.rteName)
  res.send({
    stat:i,
    file:req.query.file
  })
});
app.get('/createWay',function(req,res){
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'createWay': [ 'int',['string','string','string','string']]
  });
  var i = gpx.createWay(path.join(__dirname+"/uploads/"+req.query.file),req.query.rteName,req.query.wayName,req.query.wayLoc)
  res.send({
    stat:i,
    file:req.query.file
  })
});
app.get('/findpath',function(req,res){
  var route = ""
  var track = ""
  var gpx = ffi.Library(path.join(__dirname+'/libgpxparser'), {
    'findPathRoute': [ 'string',['string','float','float','float','float','float']],
    'findPathTrack': [ 'string',['string','float','float','float','float','float']],
  });
  route = gpx.findPathRoute(path.join(__dirname+"/uploads/"+req.query.file),req.query.slat,req.query.slon,req.query.elat,req.query.elon,req.query.delta)
  track = gpx.findPathTrack(path.join(__dirname+"/uploads/"+req.query.file),req.query.slat,req.query.slon,req.query.elat,req.query.elon,req.query.delta)
  res.send({
    route:route,
    track:track
  })
});
const mysql = require('mysql2/promise');
app.get('/connectDB', async function(req,res){
  try{
  const connection = await mysql.createConnection({
    host: req.query.host,
    user: req.query.user,
		password: req.query.pass,
		database: req.query.db
  });
  await connection.connect(function(err) {
    if (err) {
      console.error('error connecting: ', err);
      res.send({
        data:0,
      })
    }
  });
      try{
      //await connection.execute("DELETE FROM FILE")
      await connection.execute("CREATE TABLE IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT PRIMARY KEY, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL)");
      await connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT PRIMARY KEY, route_name VARCHAR(60) NOT NULL, route_len FLOAT(15,7), gpx_id INT NOT NULL, FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)");
      await connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT PRIMARY KEY, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL,point_name VARCHAR(256),route_id INT NOT NULL,FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)");
      
    }catch(err){
      console.log(err);
      await connection.end();
      res.send({
        data:1,
      })
    }
			
	
  await connection.end();
  res.send({
    data:1,
  })
}catch(err){
  console.log("Error connecting to database");
  await connection.end();
  res.send({
    data:0,
  })
}
});
app.get('/addDB', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    var t = await con.execute("SELECT 1 FROM FILE WHERE file_name = '"+req.query.file+"' LIMIT 1")
    var check = JSON.stringify(t[0]);
    if(check.localeCompare("[]")==0){
      var id = await con.execute(req.query.sql);
    }else{
      await con.end();
      console.log("File already in table")
      res.send({
        data:0
      })
    }
    await con.end();
    res.send({
      data:1,
      id: id[0].insertId
    })
  }catch(err){
    console.log("Error adding to FILE table");
    console.log(err);
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/addDBrte', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    var id = await con.execute(req.query.sql);
    await con.end();
    res.send({
      data:1,
      id: id[0].insertId,
      loc: req.query.id
    })
  }catch(err){
    console.log(err)
    console.log("Error adding to ROUTE table");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/addDBwpt', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    await con.execute(req.query.sql);
    //console.log(req.query.id)
    await con.end();
    res.send({
      data:1,
    })
  }catch(err){
    console.log(err)
    console.log("Error adding to POINT table");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/clearDB', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    await con.execute("DELETE FROM FILE")
    await con.end();
    res.send({
      data:1
    })
  }catch(err){
    console.log(err);
    console.log("Error clearing db");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/showDB', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    var fc = await con.execute("SELECT COUNT(*) AS len FROM FILE")
    var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE")
    var wc = await con.execute("SELECT COUNT(*) AS len FROM POINT")
    var str = "Database has "+fc[0][0].len+" files, "+rc[0][0].len+" routes, and "+wc[0][0].len+" points";
    await con.end();
    res.send({
      data:1,
      str: str
    })
  }catch(err){
    console.log(err);
    console.log("Error showing db");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
})
app.get('/removeFileDB', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    await con.execute("DELETE FROM FILE WHERE file_name = "+"'"+req.query.file+"'")
    await con.end();
    res.send({
      data:1
    })
  }catch(err){
    console.log(err);
    console.log("File not in table");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/printDB', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    var t = await con.execute("SELECT * FROM FILE")
    var r = await con.execute("SELECT * FROM ROUTE")
    var p = await con.execute("SELECT * FROM POINT")
    console.log(t);
    console.log(r);
    console.log(p);
    await con.end();
    res.send({
      data:1
    })
  }catch(err){
    console.log(err);
    console.log("File not in table");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pRoutes', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    if(req.query.sort.localeCompare("name")==0){
      var r = await con.execute("SELECT * FROM ROUTE ORDER BY route_name")
      var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE")
    }else{
      var r = await con.execute("SELECT * FROM ROUTE ORDER BY route_len")
      var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE")
    }
    await con.end();
    res.send({
      data:1,
      rows: r,
      len: rc[0][0].len,
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pFileRoutes', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    if(req.query.sort.localeCompare("name")==0){
      var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
      var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\" ORDER BY route_name")
      var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    }else{
      var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
      var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\" ORDER BY route_len")
      var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    }
    await con.end();
    res.send({
      data:1,
      rows: dr,
      len: rc[0][0].len
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pRPoints', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    var r = await con.execute("SELECT * FROM ROUTE WHERE route_name = \""+req.query.route+"\"")
    //console.log(r[0]);
    var dr = await con.execute("SELECT * FROM POINT WHERE route_id = \""+r[0][0].route_id+"\"ORDER BY point_index")
    //console.log(dr[0])
    var rc = await con.execute("SELECT COUNT(*) AS len FROM POINT WHERE route_id = \""+r[0][0].route_id+"\"")
    //console.log(rc[0][0].len)
    await con.end();
    res.send({
      data:1,
      rows: dr,
      len: rc[0][0].len
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pFRoutes', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    if(req.query.sort.localeCompare("name")==0){
      var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
    //console.log(r[0]);
    var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_name")
    //console.log(dr[0])
    var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    //console.log(rc[0][0].len)
    }else{
    var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
    //console.log(r[0]);
    var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_len")
    //console.log(dr[0])
    var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    //console.log(rc[0][0].len)
    }
    await con.end();
    res.send({
      data:1,
      rows: dr,
      len: rc[0][0].len
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pFPoints', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    //console.log(r[0]);
    var pr = await con.execute("SELECT * FROM POINT WHERE route_id = \""+req.query.routeId+"\"ORDER BY point_index")
    //console.log(dr[0])
    var rc = await con.execute("SELECT COUNT(*) AS len FROM POINT WHERE route_id = \""+req.query.routeId+"\"")
    //console.log(rc[0][0].len)
    await con.end();
    res.send({
      data:1,
      rows: pr,
      len: rc[0][0].len,
      rname: req.query.name,
      rlen: req.query.len
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.get('/pNRoutes', async function(req,res){
  try{
    let con = await mysql.createConnection({
      host: req.query.host,
      user: req.query.user,
      password: req.query.pass,
      database: req.query.db
    });
    if(req.query.sort.localeCompare("name")==0){
    var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
    //console.log(r[0]);
    if(req.query.dist.localeCompare("short")==0){
      var dr = await con.execute("SELECT * FROM (SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_len ASC LIMIT "+req.query.len+") AS TEMP ORDER BY route_name")
    }else{
      var dr = await con.execute("SELECT * FROM (SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_len DESC LIMIT "+req.query.len+") AS TEMP ORDER BY route_name")
    }
    //console.log(dr[0])
    var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    //console.log(rc[0][0].len)
    }else{
    var r = await con.execute("SELECT * FROM FILE WHERE file_name = \""+req.query.file+"\"")
    //console.log(r[0]);
    if(req.query.dist.localeCompare("short")==0){
      var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_len ASC LIMIT "+req.query.len)
    }else{
      var dr = await con.execute("SELECT * FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"ORDER BY route_len DESC LIMIT "+req.query.len)
    }
    var rc = await con.execute("SELECT COUNT(*) AS len FROM ROUTE WHERE gpx_id = \""+r[0][0].gpx_id+"\"")
    //console.log(rc[0][0].len)
    }
    await con.end();
    res.send({
      data:1,
      rows: dr,
      len: rc[0][0].len
    })
  }catch(err){
    console.log(err);
    console.log("Error getting route");
    try{
      await connection.end();
    }catch(err){
      res.send({
        data:0
      })
    }
    res.send({
      data:0
    })
  }
});
app.listen(portNum);
console.log('Running app at localhost: ' + portNum);