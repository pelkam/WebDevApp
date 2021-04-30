// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    //Add gpx files to table
    $.ajax({
        type: 'get',          
        url: '/upload/:filename',   
        data: {
            name: "filename",
        },
        success: function (data) {
            console.log('Successfully got files');
            var fileArr = data.files.split("|");
            fileArr.forEach(element => {
                if(element.localeCompare("")!=0&&element.localeCompare(".DS_Store")!=0&&element.split('.').pop().localeCompare("gpx")==0){
                    $('#tableFixHead #table tbody #temp').remove();
                    var str = element;
                    var sel = "<option value="+element+">"+element+"</option>";
                    $('#gpxFiles').append(sel);
                    $('#otherdatarte').append(sel);
                    $.ajax({
                        type: 'get',
                        url: '/gpxJson',
                        data: {
                            file: str
                        },
                        success: function(data){
                            if(data.gpxString.localeCompare("")==0){
                                alert("Invalid File");
                            }else{
                                var gpx = JSON.parse(data.gpxString);
                                var row = "<tr><td><a href="+str+" download >"+str+"</a></td><td>"
                                +gpx.version+"</td><td>"+gpx.creator+"</td><td>"+gpx.numWaypoints+"</td><td>"
                                +gpx.numRoutes+"</td><td>"+gpx.numTracks+"</td></tr>";
                                $('#tableFixHead #table tbody').append(row);
            
                            }
                            return;
                        },
                        fail: function(error){
                            console.log(error);
                        },
                    });
                }else{
                    console.log("Invalid File not loaded");
                }
            });

        },
        fail: function(error) {
            console.log(error); 
        }
    });
    //Upload check;
    $('#uploadForm').submit(function(e){
        var file = $('#uploadForm #div1 #file').val();
        var fname = file.replace(/^.*[\\\/]/, '')
        if(fname.split('.').pop().localeCompare("gpx")!=0){
            alert(fname+" is not a valid gpx file");
        }
    });
    //Add route/tracks to table
    $('#gpxFiles').change(function(){
        if($('#gpxFiles').val().localeCompare("temp")==0){
            $('#tableFixHead2 #table2 tbody tr').remove();
            return;
        }
        $('#tableFixHead2 #table2 tbody tr').remove();
        $('#otherdata').find('option:not(:first)').remove();
        $.ajax({
            type: 'get',
            url: '/gpxRoute',
            data: {
                file: $('#gpxFiles').val()
            },
            success: function(data){
                if(data.routeString.localeCompare("")==0){
                    console.log("Error getting route");
                    return;
                }else{
                    var route = JSON.parse(data.routeString);
                    var i;
                    for(i=0;i<route.length;++i){
                        var num = 1+i;
                        var row = "<tr><td>Route "+num+"</td><td>"+route[i].name+"</td><td>"+route[i].numPoints+"</td><td>"+route[i].len+"</td><td>"+route[i].loop+"</td></tr>";
                        $('#tableFixHead2 #table2 tbody').append(row);
                        var sel = "<option value=Route|"+num+">Route"+num+"</option>";
                        $('#otherdata').append(sel);
                    }
                }
            },
            fail: function(error){
                console.log(error);
            }
        })
        $.ajax({
            type: 'get',
            url: '/gpxTrack',
            data: {
                file: $('#gpxFiles').val()
            },
            success: function(data){
                if(data.trackString.localeCompare("")==0){
                    console.log("Error getting Track");
                    return;
                }else{
                    var track = JSON.parse(data.trackString);
                    var i;
                    for(i=0;i<track.length;++i){
                        var num = 1+i;
                        
                        var row = "<tr><td>Track "+num+"</td><td>"+track[i].name+"</td><td>"+track[i].points+"</td><td>"+track[i].len+"</td><td>"+track[i].loop+"</td></tr>";
                        $('#tableFixHead2 #table2 tbody').append(row);
                        var sel = "<option value=Track|"+num+">Track"+num+"</option>";
                        $('#otherdata').append(sel);
                    }
                }
            },
            fail: function(error){
                console.log(error);
            }
        })
    });
    //Get other data
    $('#otherdata').change(function(){
        if($('#otherdata').val().localeCompare("temp")==0){
            return;
        }
        console.log($('#gpxFiles').val());
        var id = $('#otherdata').val().split("|");
        $.ajax({
            type: 'get',
            url: '/gpxData',
            data: {
                file: $('#gpxFiles').val(),
                name: id[0],
                idNum: id[1],
            },
            success: function(data){
                var oData = JSON.parse(data.dataString);
                var str ="";
                for(var i=0;i<oData.length;++i){
                    str+= oData[i].eleName+": "+oData[i].value+"\n";
                }
                console.log(str);
                alert(str);
            },
            fail: function(error){
                console.log(error);
            }
        })
    });
    //Rename
    $('#rename').click(function(){
        var name = prompt("Name: ");
        var file = $('#gpxFiles').val();
        var id = $('#otherdata').val().split("|");
        if($('#otherdata').val().localeCompare("temp")==0){
            alert("Error no route or track selected");
            return;
        }
        $.ajax({
            type: 'get',
            url: '/rename',
            data:{
                file:file,
                name: id[0],
                idNum: id[1],
                rename:name
            },
            success: function(data){
                    updateTable(file);
                    $('#tableFixHead2 #table2 tbody tr').remove();
                    $('#otherdata').find('option:not(:first)').remove();
                    $.ajax({
                        type: 'get',
                        url: '/gpxRoute',
                        data: {
                            file: file
                        },
                        success: function(data){
                            if(data.routeString.localeCompare("")==0){
                                console.log("Error getting route");
                                return;
                            }else{
                                var route = JSON.parse(data.routeString);
                                var i;
                                for(i=0;i<route.length;++i){
                                    var num = 1+i;
                                    var row = "<tr><td>Route "+num+"</td><td>"+route[i].name+"</td><td>"+route[i].numPoints+"</td><td>"+route[i].len+"</td><td>"+route[i].loop+"</td></tr>";
                                    $('#tableFixHead2 #table2 tbody').append(row);
                                    var sel = "<option value=Route|"+num+">Route"+num+"</option>";
                                    $('#otherdata').append(sel);
                                }
                            }
                        },
                        fail: function(error){
                            console.log(error);
                        }
                    })
                    $.ajax({
                        type: 'get',
                        url: '/gpxTrack',
                        data: {
                            file: $('#gpxFiles').val()
                        },
                        success: function(data){
                            if(data.trackString.localeCompare("")==0){
                                console.log("Error getting Track");
                                return;
                            }else{
                                var track = JSON.parse(data.trackString);
                                var i;
                                for(i=0;i<track.length;++i){
                                    var num = 1+i;
                                    var row = "<tr><td>Track "+num+"</td><td>"+track[i].name+"</td><td>"+track[i].points+"</td><td>"+track[i].len+"</td><td>"+track[i].loop+"</td></tr>";
                                    $('#tableFixHead2 #table2 tbody').append(row);
                                    var sel = "<option value=Track|"+num+">Track"+num+"</option>";
                                    $('#otherdata').append(sel);
                                }
                            }
                        },
                        fail: function(error){
                            console.log(error);
                        }
                    })
            },
            fail: function(error){
                console.log(error);
            }

        })
    });
    //Create gpx
    $('div #createGpxForm').on("submit",function(e){
        e.preventDefault();
        if($('div #createGpxForm #gpxName').val()==null||$('div #createGpxForm #gpxName').val().localeCompare("")==0){
            alert("Invalid name");
            return;
        }
        if($('div #createGpxForm #gpxCreator').val()==null||$('div #createGpxForm #gpxCreator').val().localeCompare("")==0){
            alert("Invalid creator");
            return;
        }
        var name = "{\"version\":1.1,\"creator\":\""+$('div #createGpxForm #gpxCreator').val()+"\"}";
        $.ajax({
            type:'get',
            url:'/create',
            data:{
                json:name,
                file:$('div #createGpxForm #gpxName').val()+".gpx",
            },
            success:function(data){
                if(data.data==1){
                    var sel = "<option value="+data.file+">"+data.file+"</option>";
                    $('#gpxFiles').append(sel);
                    $('#otherdatarte').append(sel);
                    var str = data.file
                    $.ajax({
                        type: 'get',
                        url: '/gpxJson',
                        data: {
                            file: data.file
                        },
                        success: function(data){
                            if(data.gpxString.localeCompare("")==0){
                                alert("Invalid File");
                            }else{
                                var gpx = JSON.parse(data.gpxString);
                                var row = "<tr><td><a href="+str+" download >"+str+"</a></td><td>"
                                +gpx.version+"</td><td>"+gpx.creator+"</td><td>"+gpx.numWaypoints+"</td><td>"
                                +gpx.numRoutes+"</td><td>"+gpx.numTracks+"</td></tr>";
                                $('#tableFixHead #table tbody').append(row);
                            }
                            return;
                        },
                        fail: function(error){
                            console.log(error);
                        },
                    });
                }
            },
            fail:function(error){
                console.log(error);
            }
        })
    });
    //Create route
    $('div #createRteForm').on("submit",function(e){
        e.preventDefault();
        if($('div #createRteForm #otherdatarte').val().localeCompare("temp")==0){
            alert("Please select file");
            return;
        }
        if($('div #createRteForm #rteName').val()==null){
            alert("Invalid name");
            return;
        }
        if($('div #createRteForm #rteWay').val()==null){
            alert("Invalid way");
            return;
        }
        var wayName = new Array();
        for(var i=0;i<$('div #createRteForm #rteWay').val();++i){
            var j = i+1;
            var name = prompt("Please enter the name of waypoint "+j);
            wayName.push(name);
        }
        var rteName = $('div #createRteForm #rteName').val();
        $.ajax({
            type:'get',
            url:'/createRte',
            data:{
                rteName:rteName,
                file:$('div #createRteForm #otherdatarte').val(),
            },
            success: function(data){
                if(data.stat==1){
                    for(var i=0;i<wayName.length;++i){
                        var lat = prompt("Please enter lat");
                        var lon = prompt("Please enter lon");
                        var count = i;
                        while((lat<-90||lat>90)||(lon<-180||lon>180)){
                            alert("Invalid input");
                            var lat = prompt("Please enter lat");
                            var lon = prompt("Please enter lon")
                        }
                        var file = $('div #createRteForm #otherdatarte').val();
                        var loc = lat+'|'+lon;
                        $.ajax({
                            type:'get',
                            url:'/createWay',
                            data:{
                                wayName:wayName[i],
                                wayLoc: loc,
                                rteName:rteName,
                                file:file
                            },success: function(data){
                                updateTable(file)
                                if(data.stat==1){
                                    if(count+1 == wayName.length){
                                        $('#tableFixHead #table tbody tr').remove();
                                        $.ajax({
                                            type: 'get',            //Request type
                                            url: '/upload/:filename',   //The server endpoint we are connecting to
                                            data: {
                                                name: "filename",
                                            },
                                            success: function (data) {
                                                console.log('Successfully got files');
                                                var fileArr = data.files.split("|");
                                                fileArr.forEach(element => {
                                                    if(element.localeCompare("")!=0&&element.localeCompare(".DS_Store")!=0&&element.split('.').pop().localeCompare("gpx")==0){
                                                        $('#tableFixHead #table tbody #temp').remove();
                                                        var str = element;
                                                        var sel = "<option value="+element+">"+element+"</option>";
                                                        $('#gpxFiles').append(sel);
                                                        $('#otherdatarte').append(sel);
                                                        $.ajax({
                                                            type: 'get',
                                                            url: '/gpxJson',
                                                            data: {
                                                                file: str
                                                            },
                                                            success: function(data){
                                                                if(data.gpxString.localeCompare("")==0){
                                                                    alert("Invalid File");
                                                                }else{
                                                                    var gpx = JSON.parse(data.gpxString);
                                                                    var row = "<tr><td><a href="+str+" download >"+str+"</a></td><td>"
                                                                    +gpx.version+"</td><td>"+gpx.creator+"</td><td>"+gpx.numWaypoints+"</td><td>"
                                                                    +gpx.numRoutes+"</td><td>"+gpx.numTracks+"</td></tr>";
                                                                    $('#tableFixHead #table tbody').append(row);
                                                
                                                                }
                                                                return;
                                                            },
                                                            fail: function(error){
                                                                console.log(error);
                                                            },
                                                        });
                                                    }else{
                                                        console.log("Invalid File not loaded");
                                                    }
                                                });
                                    
                                            },
                                            fail: function(error) {
                                                console.log(error); 
                                            }
                                        })
                                    if($('#gpxFiles').val().localeCompare($('div #createRteForm #otherdatarte').val())==0){
                                        $('#tableFixHead2 #table2 tbody tr').remove();
                                        $('#otherdata').find('option:not(:first)').remove();
                                        $.ajax({
                                            type: 'get',
                                            url: '/gpxRoute',
                                            data: {
                                            file: file
                                        },
                                        success: function(data){
                                            if(data.routeString.localeCompare("")==0){
                                                console.log("Error getting route");
                                                return;
                                            }else{
                                                var route = JSON.parse(data.routeString);
                                                var i;
                                                for(i=0;i<route.length;++i){
                                                    var num = 1+i;
                                                    var row = "<tr><td>Route "+num+"</td><td>"+route[i].name+"</td><td>"+route[i].numPoints+"</td><td>"+route[i].len+"</td><td>"+route[i].loop+"</td></tr>";
                                                    $('#tableFixHead2 #table2 tbody').append(row);
                                                    var sel = "<option value=Route|"+num+">Route"+num+"</option>";
                                                    $('#otherdata').append(sel);
                                                }
                                            }
                                        },
                                        fail: function(error){
                                            console.log(error);
                                        }
                                        })
                                        $.ajax({
                                            type: 'get',
                                            url: '/gpxTrack',
                                            data: {
                                                file: $('#gpxFiles').val()
                                        },
                                        success: function(data){
                                        if(data.trackString.localeCompare("")==0){
                                            console.log("Error getting Track");
                                            return;
                                        }else{
                                            var track = JSON.parse(data.trackString);
                                            var i;
                                            for(i=0;i<track.length;++i){
                                                var num = 1+i;
                                                var row = "<tr><td>Track "+num+"</td><td>"+track[i].name+"</td><td>"+track[i].points+"</td><td>"+track[i].len+"</td><td>"+track[i].loop+"</td></tr>";
                                                $('#tableFixHead2 #table2 tbody').append(row);
                                                var sel = "<option value=Track|"+num+">Track"+num+"</option>";
                                                $('#otherdata').append(sel);
                                            }
                                        }
                                        },
                                        fail: function(error){
                                            console.log(error);
                                        }
                                        })
                                    }
                                }
                                }else{
                                    alert("Failed to add waypoint");
                                    return;
                                }
                            }

                        });
                    }
                }else{
                    alert("Failed to add route");
                    return;
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });
    //Display all paths
    $('div #paths').on("submit",function(e){
        e.preventDefault();
        if($('div #paths #slon').val()==null||$('div #paths #slon').val().localeCompare("")==0||$('div #paths #slon').val()<-180||$('div #paths #slon').val()>180){
            alert("Invalid start lon");
            return;
        }
        if($('div #paths #slat').val()==null||$('div #paths #slat').val().localeCompare("")==0||$('div #paths #slat').val()<-90||$('div #paths #slat').val()>90){
            alert("Invalid start lat");
            return;
        }
        if($('div #paths #elon').val()==null||$('div #paths #elon').val().localeCompare("")==0||$('div #paths #elon').val()<-180||$('div #paths #elon').val()>180){
            alert("Invalid end lon");
            return;
        }
        if($('div #paths #elat').val()==null||$('div #paths #elat').val().localeCompare("")==0||$('div #paths #elat').val()<-90||$('div #paths #elat').val()>90){
            alert("Invalid end lat");
            return;
        }
        if($('div #paths #del').val()==null||$('div #paths #del').val().localeCompare("")==0||$('div #paths #del').val()<0){
            alert("Invalid delta");
            return;
        }
        $('#tableFixHead3 #table3 tbody tr').remove();
        $.ajax({
            type: 'get',            
            url: '/upload/:filename',   
            data: {
                name: "filename",
            },
            success: function (data) {
                var fileArr = data.files.split("|");
                var routeCount = 0
                var trackCount = 0
                for(var i=0;i<fileArr.length;++i){
                    var element = fileArr[i]
                    if(element.localeCompare("")!=0&&element.localeCompare(".DS_Store")!=0&&element.split('.').pop().localeCompare("gpx")==0){
                        $.ajax({
                            type: 'get',            
                            url: '/findpath',
                            data: {
                                file: element,
                                slat: $('div #paths #slat').val(),
                                slon: $('div #paths #slon').val(),
                                elat: $('div #paths #elat').val(),
                                elon: $('div #paths #elon').val(),
                                delta: $('div #paths #del').val(),
                            },
                            success: function(data){
                                if(data.route.localeCompare("[]")!=0){
                                    var route = JSON.parse(data.route)
                                    for(var i=0;i<route.length;++i){
                                        var num = i+routeCount+1;
                                        var row = "<tr><td>Route "+num+"</td><td>"+route[i].name+"</td><td>"+route[i].numPoints+"</td><td>"+route[i].len+"</td><td>"+route[i].loop+"</td></tr>";
                                        $('#tableFixHead3 #table3 tbody').append(row);
                                    }
                                    routeCount+=route.length;
                                }
                                if(data.track.localeCompare("[]")!=0){
                                    var track = JSON.parse(data.track)
                                    for(var i=0;i<track.length;++i){
                                        var num = i+trackCount+1;
                                        var row = "<tr><td>Track "+num+"</td><td>"+track[i].name+"</td><td>"+track[i].points+"</td><td>"+track[i].len+"</td><td>"+track[i].loop+"</td></tr>";
                                        $('#tableFixHead3 #table3 tbody').append(row);
                                    }
                                    trackCount+=track.length;
                                }
                            },
                            fail: function(error){
                                console.log(error);
                            }
                        });
                    }
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });
    /*********SQL*********/
    $('div #connectDB').on("submit",function(e){
        e.preventDefault();
        n = $('#uname').val();
		p = $('#pass').val();
		d = $('#db').val();
        $.ajax({
			type: 'get',            	  	
			url: '/connectDB',
			data: {
				host: h,
				user: n,
				pass: p,
				db: d
			},success: function(data){
                if(data.data==0){
                    alert("Error getting database");
                    n = ""
                    p = ""
                    d = ""
                    return;
                }else{
                    displayDBstat();
                }
            }
        })
    })
    $('div #logoutDB').click(function(){
        d =""
        n =""
        d =""
        alert("Successfully logged out")
        return;
    })
    $('div #uploadDB').click(function(){
        if(d.localeCompare("")==0){
            alert("Connect to database first");
            return;
        }
        if($("#gpxFiles option").length<=1){
            console.log("No valid files to add to table");
        }else{
            console.log("Adding to table"); 
            $.ajax({
            type: 'get',          
            url: '/upload/:filename',   
            data: {
            name: "filename",
            },
            success: function (data) {
                console.log('Successfully got files');
                var fileArr = data.files.split("|");
                
                fileArr.forEach(element => {
                if(element.localeCompare("")!=0&&element.localeCompare(".DS_Store")!=0&&element.split('.').pop().localeCompare("gpx")==0){
                    var str = element;
                    $.ajax({
                        type: 'get',
                        url: '/gpxJson',
                        data: {
                            file: str
                        },
                        success: function(data){
                            if(data.gpxString.localeCompare("")==0){
                                alert("Invalid File");
                            }else{
                                var gpx = JSON.parse(data.gpxString);
                                var sql = "INSERT INTO FILE (file_name,ver,creator) VALUES ("+"'"+str+"'"+","+"'"+gpx.version+"'"+","+"'"+gpx.creator+"'"+")"
                                //console.log(sql);
                                $.ajax({
                                    type: 'get',
                                    url: '/addDB',
                                    data: {
                                        host: h,
                                        user: n,
                                        pass: p,
                                        db: d,
                                        sql: sql,
                                        file:str
                                },
                                success:function(data){
                                    var gpxid = data.id;
                                    var un = 1;
                                    if(data.data ==1){
                                        $.ajax({
                                            type: 'get',
                                            url: '/gpxRoute',
                                            data: {
                                            file: str
                                            },
                                            success: function(data){
                                                if(data.routeString.localeCompare("")==0){
                                                    console.log("Error getting route");  
                                                }else{
                                                    var route = JSON.parse(data.routeString);
                                                    var count = 0;
                                                    for(var i=0;i<route.length;++i){
                                                        //console.log(count)
                                                        if(route[count].name.localeCompare("None")==0){
                                                            route[count].name = "unamed Route "+un;
                                                            ++un;
                                                        }
                                                        var sqlrte = "INSERT INTO ROUTE (route_name,route_len,gpx_id) VALUES ("+"'"+route[count].name+"'"+","+"'"+route[count].len+"'"+","+"'"+gpxid+"'"+")"
                                                        $.ajax({
                                                            type: 'get',
                                                            url: '/addDBrte',
                                                            data:{
                                                                host: h,
                                                                user: n,
                                                                pass: p,
                                                                db: d,
                                                                sql: sqlrte,
                                                                id: count
                                                            },
                                                            success: function(data){
                                                                var routeId = data.id;
                                                                var id = data.loc;
                                                                var wun = 1;
                                                                $.ajax({
                                                                    type: 'get',
                                                                    url: '/gpxWaypoint',
                                                                    data:{
                                                                        file: str,
                                                                        rtId: id,
                                                                    },
                                                                    success: function(data){
                                                                        if(data.wptString.localeCompare("")!=0){
                                                                            var wpt = JSON.parse(data.wptString);
                                                                            var wCount = 0;
                                                                            for(var i=0;i<wpt.length;++i){
                                                                                var index = wCount +1;
                                                                                if(wpt[wCount].name.localeCompare("None")==0){
                                                                                    wpt[wCount].name = "unnamed point "+wun;
                                                                                    ++wun;
                                                                                }
                                                                                var wptsql = "INSERT INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES ("+"'"+index+"'"+","+"'"+wpt[wCount].lat+"'"+","+"'"+wpt[wCount].lon+"'"+","+"'"+wpt[wCount].name+"'"+","+"'"+routeId+"'"+")"
                                                                                //console.log("Route id: "+routeId);
                                                                                $.ajax({
                                                                                    type: 'get',
                                                                                    url: '/addDBwpt',
                                                                                    data:{
                                                                                        host: h,
                                                                                        user: n,
                                                                                        pass: p,
                                                                                        db: d,
                                                                                        sql: wptsql,
                                                                                        id: routeId
                                                                                    },
                                                                                    success: function(data){
                                                                                        if(data.data==1){
                                                                                            console.log("Added point")
                                                                                        }
                                                                                    },
                                                                                    fail: function(error){
                                                                                        console.log(error)
                                                                                    }
                                                                                                    
                                                                                })
                                                                                //console.log(wCount)
                                                                            ++wCount;
                                                                            }
                                                                                                
                                                                        }
                                                                    },
                                                                    fail: function(error){
                                                                        console.log(error);
                                                                    }
                                                                })
                                                            },
                                                            fail: function(error){
                                                                console.log(error);
                                                            }
                                                                                
                                                        })
                                                    ++count;
                                                    }
                                                }
                                            },
                                            fail: function(error){
                                                console.log(error);
                                            }
                                        })
                                    }else{
                                        console.log("File already in table");
                                    }
                                },
                                fail: function(error){
                                    console.log(error);
                                }
                                });
                            }
                        },
                        fail: function(error){
                            console.log(error);
                        },
                    });
                }else{
                    console.log("Invalid File not loaded");   
                }
            });
            },
            fail: function(error) {
                console.log(error); 
            }
            });
        }
        alert("All data added")
        displayDBstat();
});
    $("div #clearDB").click(function(){
        if(d.localeCompare("")==0){
            alert("Connect to a database first");
            return;
        }
        $.ajax({
            type: 'get',
            url: '/clearDB',
            data:{
                host: h,
                pass: p,
                user: n,
                db: d
            },
            success: function(data){
                if(data.data==1){
                    alert("Successfuly cleared database")
                    displayDBstat();
                }
            },
            fail: function(error){

            }
        })
    });
    $("div #statDB").click(function(){
        if(d.localeCompare("")==0){
            alert("Connect to a database first");
            return;
        }
        displayDBstat();
    });
    $("div #printDB").click(function(){
        $.ajax({
            type: 'get',
            url: '/printDB',
            data:{
                host: h,
                pass: p,
                user: n,
                db: d
            },
        })
    });
    function displayDBstat(){
        $.ajax({
            type: 'get',
            url: '/showDB',
            data:{
                host: h,
                pass: p,
                user: n,
                db: d
            },
            success: function(data){
                alert(data.str);
            },
            fail: function(error){
                console.log(error)
            }
        })
    }
    function updateTable(file){
        var str = file
        if(d.localeCompare("")==0){
            return;
        }
        $.ajax({
            type: 'get',
            url: '/removeFileDB',
            data: {
                host: h,
                user: n,
                pass: p,
                db: d,
                file: file
            },
            success: function(data){

            },
            fail: function(error){
                console.log(error)
            }
        })
        $.ajax({
            type: 'get',
            url: '/gpxJson',
            data: {
                file: str
            },
            success: function(data){
                if(data.gpxString.localeCompare("")==0){
                    alert("Invalid File");
                }else{
                    var gpx = JSON.parse(data.gpxString);
                    var sql = "INSERT INTO FILE (file_name,ver,creator) VALUES ("+"'"+str+"'"+","+"'"+gpx.version+"'"+","+"'"+gpx.creator+"'"+")"
                    //console.log(sql);
                    $.ajax({
                        type: 'get',
                        url: '/addDB',
                        data: {
                            host: h,
                            user: n,
                            pass: p,
                            db: d,
                            sql: sql,
                            file:str
                        },
                        success:function(data){
                            var gpxid = data.id;
                            var un = 1;
                            if(data.data ==1){
                                $.ajax({
                                    type: 'get',
                                    url: '/gpxRoute',
                                    data: {
                                    file: str
                                },
                                success: function(data){
                                    if(data.routeString.localeCompare("")==0){
                                        console.log("Error getting route");
                                        
                                    }else{
                                        var route = JSON.parse(data.routeString);
                                        var count = 0;
                                        for(var i=0;i<route.length;++i){
                                            //console.log(count)
                                            if(route[count].name.localeCompare("None")==0){
                                                route[count].name = "unamed Route "+un;
                                                ++un;
                                            }
                                            var sqlrte = "INSERT INTO ROUTE (route_name,route_len,gpx_id) VALUES ("+"'"+route[count].name+"'"+","+"'"+route[count].len+"'"+","+"'"+gpxid+"'"+")"
                                            $.ajax({
                                                type: 'get',
                                                url: '/addDBrte',
                                                data:{
                                                    host: h,
                                                    user: n,
                                                    pass: p,
                                                    db: d,
                                                    sql: sqlrte,
                                                    id: count
                                                },
                                                success: function(data){
                                                    var routeId = data.id;
                                                    var id = data.loc;
                                                    var wun = 1;
                                                    $.ajax({
                                                        type: 'get',
                                                        url: '/gpxWaypoint',
                                                        data:{
                                                            file: str,
                                                            rtId: id,
                                                        },
                                                        success: function(data){
                                                            if(data.wptString.localeCompare("")!=0){
                                                                var wpt = JSON.parse(data.wptString);
                                                                var wCount = 0;
                                                                
                                                                for(var i=0;i<wpt.length;++i){
                                                                    var index = wCount +1;
                                                                    if(wpt[wCount].name.localeCompare("None")==0){
                                                                        wpt[wCount].name = "unnamed point "+wun;
                                                                        ++wun;
                                                                    }
                                                                    var wptsql = "INSERT INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES ("+"'"+index+"'"+","+"'"+wpt[wCount].lat+"'"+","+"'"+wpt[wCount].lon+"'"+","+"'"+wpt[wCount].name+"'"+","+"'"+routeId+"'"+")"
                                                                    //console.log("Route id: "+routeId);
                                                                    $.ajax({
                                                                        type: 'get',
                                                                        url: '/addDBwpt',
                                                                        data:{
                                                                            host: h,
                                                                            user: n,
                                                                            pass: p,
                                                                            db: d,
                                                                            sql: wptsql,
                                                                            id: routeId
                                                                        },
                                                                        success: function(data){
                                                                            if(data.data==1){
                                                                                console.log("Added point")
                                                                            }
                                                                        
                                                                        },
                                                                        fail: function(error){
                                                                            console.log(error)
                                                                        }
                                                                    
                                                                    })
                                                                //console.log(wCount)
                                                                ++wCount;
                                                                }
                                                                
                                                            }
                                                        },
                                                        fail: function(error){
                                                            console.log(error);
                                                        }
                                                    })
                                                },
                                                fail: function(error){
                                                    console.log(error);
                                                }
                                                
                                            })
                                            ++count;
                                        }
                                    }
                                },
                                fail: function(error){
                                    console.log(error);
                                }
                                })
                            }else{
                                console.log("File already in table");
                            }
                        },
                        fail: function(error){
                            console.log(error);
                        }
                    });
                }
            },
            fail: function(error){
                console.log(error);
            },
        });
    }
    $("div #query").change(function(){
        //console.log($("div #query").val());
        $("div #header h5").remove();
        $("div #tableFixHead4 #table4 tbody tr").remove();
        $("div #tableFixHead4 #table4 thead tr").remove();
        $("div #qmenu form label").remove();
        $("div #qmenu form input").remove();
        $("div #qmenu form br").remove();
        $("div #qmenu form #len").remove();
        $("div #qmenu form #sort option").remove();
        $("div #qmenu form #sort").append("<br>");
        if($("div #query").val().localeCompare("printRoutes")==0){
            $("div #qmenu form #sort").append("<option value=\"len\">Sort by length</option>");
            $("div #qmenu form #sort").append("<option value=\"name\">Sort by name</option>");
        }
        if($("div #query").val().localeCompare("printFileRoutes")==0){
            $("div #qmenu form").append("<br><label for=\"fileName\">File:</label><br>");
            $("div #qmenu form").append("<input type =\"text\" id = \"fileName\" value = \"\"></input><br>");
            $("div #qmenu form #sort").append("<option value=\"len\">Sort by length</option>");
            $("div #qmenu form #sort").append("<option value=\"name\">Sort by name</option>");
        }
        if($("div #query").val().localeCompare("printRoutePoint")==0){
            $("div #qmenu form").append("<br><label for=\"rteName\">Route:</label><br>");
            $("div #qmenu form").append("<input type =\"text\" id = \"rteName\" value = \"\"></input><br>");
        }
        if($("div #query").val().localeCompare("printFilePoint")==0){
            $("div #qmenu form").append("<br><label for=\"fileName\">File:</label><br>");
            $("div #qmenu form").append("<input type =\"text\" id = \"fileName\" value = \"\"></input><br>");
            $("div #qmenu form #sort").append("<option value=\"len\">Sort by length</option>");
            $("div #qmenu form #sort").append("<option value=\"name\">Sort by name</option>");
        }
        if($("div #query").val().localeCompare("printNPath")==0){
            $("div #qmenu form").append("<br><label for=\"fileName\">File:</label><br>");
            $("div #qmenu form").append("<input type =\"text\" id = \"fileName\" value = \"\"></input><br>");
            $("div #qmenu form").append("<label for=\"N\">Number of routes:</label><br>");
            $("div #qmenu form").append("<input type =\"text\" id = \"N\" value = \"\"></input><br>");
            $("div #qmenu form #sort").append("<option value=\"len\">Sort by length</option>");
            $("div #qmenu form #sort").append("<option value=\"name\">Sort by name</option>");
            $("div #qmenu form").append("<select id = \"len\"></select>");
            $("div #qmenu form #len").append("<option value=\"short\">Shortest</option>");
            $("div #qmenu form #len").append("<option value=\"long\">Longest</option>");
        }
    })
    $("div div #querySub").click(function(){
        $("div #header h5").remove();
        $("div #tableFixHead4 #table4 tbody tr").remove();
        $("div #tableFixHead4 #table4 thead tr").remove();
        if(d.localeCompare("")==0){
            alert("Connect to database first");
            return;
        }
        if($("div #query").val().localeCompare("printRoutes")==0){
            $.ajax({
                type: 'get',
                url: '/pRoutes',
                data:{
                    host: h,
                    user: n,
                    pass: p,
                    db: d,
                    sort: $("div #qmenu form #sort").val()
                },
                success: function(data){
                    //console.log(data.len)
                    if(data.data==0){
                        console.log("Error getting routes")
                        return
                    }
                    $("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Route name</th><th scope=\"col\">Route length</th></tr>");
                    var rcount = 0
                    for(var i=0;i<data.len;++i){
                        $("div #tableFixHead4 #table4 tbody").append("<tr><td>"+data.rows[0][rcount].route_name+"</td><td>"+data.rows[0][rcount].route_len+" m</td></tr>");
                        ++rcount;
                    }
                    console.log("successfully got routes")
                },
                fail: function(error){
                    console.log(error)
                }
            })
        }
        if($("div #query").val().localeCompare("printFileRoutes")==0){
            var fi = $("div #qmenu form #fileName").val();
            if($("div #qmenu form #fileName").val().localeCompare("")==0||fi.split('.').pop().localeCompare("gpx")!=0){
                alert("Enter valid file include .gpx extension")
                return
            }
            $.ajax({
                type: 'get',
                url: '/pFileRoutes',
                data:{
                    host: h,
                    user: n,
                    pass: p,
                    db: d,
                    sort: $("div #qmenu form #sort").val(),
                    file: $("div #qmenu form #fileName").val()
                },
                success: function(data){
                    if(data.data==0){
                        console.log("Error getting routes")
                        return;
                    }
                    $("div #header").append("<h5>File: "+$("div #qmenu form #fileName").val()+"</h5>");
                    $("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Route name</th><th scope=\"col\">Route length</th></tr>");
                    var rcount = 0
                    for(var i=0;i<data.len;++i){
                        $("div #tableFixHead4 #table4 tbody").append("<tr><td>"+data.rows[0][rcount].route_name+"</td><td>"+data.rows[0][rcount].route_len+" m</td></tr>");
                        ++rcount;
                    }
                    console.log("Succesfully got routes")
                },
                fail: function(error){
                    console.log(error)
                }
            });
        }
        if($("div #query").val().localeCompare("printRoutePoint")==0){
            if($("div #qmenu form #rteName").val().localeCompare("")==0){
                alert("Enter valid route name")
                return
            }
            $.ajax({
                type: 'get',
                url: '/pRPoints',
                data:{
                    host: h,
                    user: n,
                    pass: p,
                    db: d,
                    route: $("div #qmenu form #rteName").val()
                },
                success: function(data){
                    if(data.data==0){
                        console.log("Error getting routes")
                        return;
                    }
                    $("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Point index</th><th scope=\"col\">Point name</th><th scope=\"col\">Latitude</th><th scope=\"col\">Longitude</th></tr>");
                    //$("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Route name</th><th scope=\"col\">Route length</th></tr>");
                    var rcount = 0
                    for(var i=0;i<data.len;++i){
                        $("div #tableFixHead4 #table4 tbody").append("<tr><td>"+data.rows[0][rcount].point_index+"</td><td>"+data.rows[0][rcount].point_name+"</td><td>"+data.rows[0][rcount].latitude+"</td><td>"+data.rows[0][rcount].longitude+"</td></tr>");
                        ++rcount;
                    }
                    console.log("Succesfully got routes")
                },
                fail: function(error){
                    console.log(error);
                }
            })
        }
        if($("div #query").val().localeCompare("printFilePoint")==0){
            var fi = $("div #qmenu form #fileName").val();
            if($("div #qmenu form #fileName").val().localeCompare("")==0||fi.split('.').pop().localeCompare("gpx")!=0){
                alert("Enter valid file include .gpx extension")
                return
            }
            $.ajax({
                type: 'get',
                url: '/pFRoutes',
                data:{
                    host: h,
                    user: n,
                    pass: p,
                    db: d,
                    file: $("div #qmenu form #fileName").val(),
                    sort: $("div #qmenu form #sort").val()
                },
                success: function(data){
                    var scount = 0;
                    if(data.data==0){
                        console.log("Error getting route")
                    }
                    $("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Route name</th><th scope=\"col\">Route len</th><th scope=\"col\">Point index</th><th scope=\"col\">Point name</th><th scope=\"col\">Latitude</th><th scope=\"col\">Longitude</th></tr>");
                    for(var i=0;i<data.len;++i){
                        var rname = data.rows[0][scount].route_name;
                        var rlen = data.rows[0][scount].route_len;
                        $.ajax({
                            type: 'get',
                            url: '/pFPoints',
                            data:{
                                host: h,
                                user: n,
                                pass: p,
                                db: d,
                                name: rname,
                                len: rlen,
                                routeId: data.rows[0][scount].route_id
                            },
                            success: function(data){
                                if(data.data==0){
                                    console.log("Error getting point")
                                }
                                var pcount =0;
                                for(var j=0;j<data.len;++j){
                                    $("div #tableFixHead4 #table4 tbody").append("<tr><td>"+data.rname+"</td><td>"+data.rlen+"</td><td>"+data.rows[0][pcount].point_index+"</td><td>"+data.rows[0][pcount].point_name+"</td><td>"+data.rows[0][pcount].latitude+"</td><td>"+data.rows[0][pcount].longitude+"</td></tr>");
                                    ++pcount;
                                }
                            },
                            fail: function(error){
                                console.log(error)
                            }
                        })
                        ++scount;
                    }
                },
                fail: function(error){

                }
            });
        }
        if($("div #query").val().localeCompare("printNPath")==0){
            var fi = $("div #qmenu form #fileName").val();
            if($("div #qmenu form #N").val().localeCompare("")==0){
                alert("Enter valid number of routes")
                return
            }
            if($("div #qmenu form #fileName").val().localeCompare("")==0||fi.split('.').pop().localeCompare("gpx")!=0){
                alert("Enter valid file include .gpx extension")
                return
            }
            var nlen = $("div #qmenu form #N").val()
            $.ajax({
                type: 'get',
                url: '/pNRoutes',
                data:{
                    host: h,
                    user: n,
                    pass: p,
                    db: d,
                    file: $("div #qmenu form #fileName").val(),
                    sort: $("div #qmenu form #sort").val(),
                    len: nlen,
                    dist: $("div #qmenu form #len").val()
                },
                success: function(data){
                    $("div #header").append("<h5>File: "+$("div #qmenu form #fileName").val()+"</h5>");
                    $("div #tableFixHead4 #table4 thead").append("<tr><th scope=\"col\">Route name</th><th scope=\"col\">Route length</th></tr>");
                    if(data.len < nlen){
                        nlen = data.len
                    }
                    var count = 0;
                    for(var i=0;i<nlen;++i){
                        $("div #tableFixHead4 #table4 tbody").append("<tr><td>"+data.rows[0][count].route_name+"</td><td>"+data.rows[0][count].route_len+" m</td></tr>");
                        ++count;
                    }
                },
                fail: function(error){
                    console.log(error);
                }
            });
        }
        
    })
    var n = ""
    var p = ""
    var d = ""
    var h = 'dursley.socs.uoguelph.ca';
});
