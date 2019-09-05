Option Strict On
Imports System.Runtime.InteropServices
Public Class Form1
    Friend WithEvents BraillePanel1 As New BraillePanel With {.Parent = Me, .BackColor = Color.Black, .ForeColor = Color.White}
    Private Sub TextBox1_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles TextBox1.TextChanged
        BraillePanel1.Text = TextBox1.Text
    End Sub
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        doLayout()
    End Sub
    Private Sub Form1_SizeChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.SizeChanged
        doLayout()
        BraillePanel1.UpdateBraille()
    End Sub
    Sub doLayout()
        NumericUpDown1.Top = Me.ClientRectangle.Height - NumericUpDown1.Height
        NumericUpDown1.Left = 0
        NumericUpDown1.Width = Me.ClientRectangle.Width \ 2
        CheckBox1.Left = NumericUpDown1.Left + NumericUpDown1.Width + 5
        CheckBox1.Width = NumericUpDown1.Width
        CheckBox1.Top = NumericUpDown1.Top
        TextBox1.Left = 0
        TextBox1.Width = Me.ClientRectangle.Width
        TextBox1.Top = NumericUpDown1.Top - TextBox1.Height
        BraillePanel1.Location = New Point(0, 0)
        BraillePanel1.Width = Me.ClientRectangle.Width
        BraillePanel1.Height = TextBox1.Top
    End Sub
    Private Sub NumericUpDown1_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NumericUpDown1.ValueChanged
        BraillePanel1.FontSize = CInt(NumericUpDown1.Value)
    End Sub
    Private Sub CheckBox1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CheckBox1.CheckedChanged
        BraillePanel1.DrawBlanks = CheckBox1.Checked
    End Sub
End Class
Public Class BraillePanel
    Inherits Panel
    Private Property _text As String
    Private Property _fontSize As Integer = 10
    Private Property _drawBlanks As Boolean
    Friend WithEvents pnlDisplay As New dbPictureBox
    Public Shadows Event TextChanged(ByVal sender As Object, ByVal e As EventArgs)
    Public Shadows Property Text As String
        Get
            Return _text
        End Get
        Set(ByVal value As String)
            _text = value
            RaiseEvent TextChanged(Me, New EventArgs)
        End Set
    End Property
    Public Property DrawBlanks As Boolean
        Get
            Return _drawBlanks
        End Get
        Set(ByVal value As Boolean)
            _drawBlanks = value
            UpdateBraille()
        End Set
    End Property
    Public Property FontSize As Integer
        Get
            Return _fontSize
        End Get
        Set(ByVal value As Integer)
            _fontSize = value
            UpdateBraille()
        End Set
    End Property
    Sub New()
        MyBase.DoubleBuffered = True
        MyBase.AutoScroll = True
        MyBase.Controls.Add(pnlDisplay)
        MyBase.Cursor = Cursors.IBeam
    End Sub
    Sub New(ByVal text As String)
        Me.Text = text
        Me.AutoScroll = True
        Me.Controls.Add(pnlDisplay)
        Me.Cursor = Cursors.IBeam
    End Sub
    Sub Text_Changed(ByVal sender As Object, ByVal e As EventArgs) Handles Me.TextChanged
        UpdateBraille()
    End Sub
    Public Sub UpdateBraille()
        pnlDisplay.BackgroundImageLayout = ImageLayout.None
        pnlDisplay.BackgroundImage = Me.ToBraille(Me.Text, Me.FontSize, Me.BackColor, Me.ForeColor, Me.DrawBlanks)
        pnlDisplay.Location = New Point(10, 10)
        pnlDisplay.Size = pnlDisplay.BackgroundImage.Size
    End Sub
    Private Function ToBraille(ByVal text As String, ByVal fontSize As Integer, ByVal backColor As Color, ByVal foreColor As Color, ByVal drawBlanks As Boolean) As Bitmap
        Dim lines() As String = {}
        Dim dotSize As Integer = fontSize - 2
        Dim pad As Integer = dotSize \ 2
        Dim characterWidth As Integer = fontSize * 2 + (pad * 2)
        Dim characterHeight As Integer = fontSize * 3 + (pad * 2)
        Dim crColor As Color = Color.FromArgb(255, 255, 255, 0)
        Dim lfColor As Color = Color.FromArgb(255, 0, 255, 255)
        Dim unknownColor As Color = Color.FromArgb(255, 255, 0, 255)
        Try
            lines = text.Split(CChar(vbCr))
        Catch ex As Exception
            Dim bm As New Bitmap(1, 1)
            Dim g As Graphics = Graphics.FromImage(bm)
            g.Clear(MyBase.BackColor)
            Return bm
        End Try
        Dim maxSize As New Size(0, lines.Count * characterHeight)
        For Each line As String In lines
            If (line.Length * characterWidth) > maxSize.Width Then maxSize.Width = (line.Length * characterWidth)
        Next
        Dim images As New List(Of Image)
        Dim finalBM As Bitmap
        Try
            finalBM = New Bitmap(maxSize.Width, maxSize.Height)
        Catch ex As Exception
            Dim bm As New Bitmap(1, 1)
            Dim g As Graphics = Graphics.FromImage(bm)
            g.Clear(MyBase.BackColor)
            Return bm
        End Try
        Dim finalG As Graphics = Graphics.FromImage(finalBM)
        finalG.SmoothingMode = Drawing2D.SmoothingMode.None
        finalG.Clear(backColor)
        If text = "" Then Return finalBM
        For Each s As String In text
            Dim keyCode As String = String.Empty
            Dim bm As New Bitmap(characterWidth, characterHeight)
            Dim g As Graphics = Graphics.FromImage(bm)
            Dim x As Integer = 0
            Dim y As Integer = -fontSize
            g.SmoothingMode = Drawing2D.SmoothingMode.AntiAlias
            g.Clear(backColor)
            Select Case s.ToLower
                Case "a" : keyCode = "100000"
                Case "b" : keyCode = "110000"
                Case "c" : keyCode = "100100"
                Case "d" : keyCode = "100110"
                Case "e" : keyCode = "100010"
                Case "f" : keyCode = "110100"
                Case "g" : keyCode = "110110"
                Case "h" : keyCode = "110010"
                Case "i" : keyCode = "010100"
                Case "j" : keyCode = "010110"
                Case "k" : keyCode = "101000"
                Case "l" : keyCode = "111000"
                Case "m" : keyCode = "101100"
                Case "n" : keyCode = "101110"
                Case "o" : keyCode = "101010"
                Case "p" : keyCode = "111100"
                Case "q" : keyCode = "111110"
                Case "r" : keyCode = "111010"
                Case "s" : keyCode = "011100"
                Case "t" : keyCode = "011110"
                Case "u" : keyCode = "101001"
                Case "v" : keyCode = "111001"
                Case "w" : keyCode = "010111"
                Case "x" : keyCode = "101101"
                Case "y" : keyCode = "101111"
                Case "z" : keyCode = "101011"
                Case "1" : keyCode = "010000"
                Case "2" : keyCode = "011000"
                Case "3" : keyCode = "010010"
                Case "4" : keyCode = "010011"
                Case "5" : keyCode = "010001"
                Case "6" : keyCode = "011010"
                Case "7" : keyCode = "011011"
                Case "8" : keyCode = "011001"
                Case "9" : keyCode = "001010"
                Case "0" : keyCode = "001011"
                Case "&" : keyCode = "111101"
                Case "=" : keyCode = "111111"
                Case "(" : keyCode = "111011"
                Case "!" : keyCode = "011101"
                Case ")" : keyCode = "011111"
                Case "*" : keyCode = "100001"
                Case "<" : keyCode = "110001"
                Case "%" : keyCode = "100101"
                Case "?" : keyCode = "100111"
                Case ":" : keyCode = "100011"
                Case "$" : keyCode = "110101"
                Case "]" : keyCode = "110111"
                Case "\" : keyCode = "110011"
                Case "[" : keyCode = "010101"
                Case "/" : keyCode = "001100"
                Case "+" : keyCode = "001101"
                Case "#" : keyCode = "001111"
                Case ">" : keyCode = "001110"
                Case "'" : keyCode = "001000"
                Case "-" : keyCode = "001001"
                Case "@" : keyCode = "000100"
                Case "^" : keyCode = "000110"
                Case "_" : keyCode = "000111"
                Case """" : keyCode = "000010"
                Case "." : keyCode = "000101"
                Case ";" : keyCode = "000011"
                Case "," : keyCode = "000001"
                Case " "
                    g.SmoothingMode = Drawing2D.SmoothingMode.None
                    g.Clear(backColor)
                    images.Add(bm)
                    Continue For
                Case vbCr
                    g.SmoothingMode = Drawing2D.SmoothingMode.None
                    g.Clear(Color.White)
                    bm.SetPixel(0, 0, crColor)
                    images.Add(bm)
                    Continue For
                Case vbLf
                    g.SmoothingMode = Drawing2D.SmoothingMode.None
                    g.Clear(Color.White)
                    bm.SetPixel(0, 0, lfColor)
                    images.Add(bm)
                    Continue For
                Case Else
                    g.SmoothingMode = Drawing2D.SmoothingMode.None
                    g.Clear(Color.White)
                    bm.SetPixel(0, 0, unknownColor)
                    images.Add(bm)
                    Continue For
            End Select
            For Each s2 As String In keyCode.ToString
                y += fontSize
                If y > (fontSize * 2) Then
                    y = 0 : x += fontSize
                End If
                If s2 = "1" Then
                    g.FillEllipse(New SolidBrush(foreColor), _
                      New Rectangle(New Point(x + pad, y + pad), _
                      New Size(dotSize, dotSize)))
                Else
                    If drawBlanks Then
                        g.DrawEllipse(New Pen(New SolidBrush(foreColor)), New Rectangle(New Point(x + pad, y + pad), New Size(dotSize, dotSize)))
                    End If
                End If

            Next
            images.Add(bm)
        Next
        Dim left As Integer = -images(0).Width
        Dim top As Integer = 0
        For Each Image As Bitmap In images
            Dim color As Color = Image.GetPixel(0, 0)
            Select Case color
                Case crColor
                    left = -Image.Width : top += Image.Height
                    Continue For
                Case lfColor
                    Continue For
                Case unknownColor
                    '    MsgBox("unknown!")
                    Continue For
                Case Else

            End Select
            left += Image.Width
            If (left + Image.Width) > maxSize.Width Then
                left = 0 : top += Image.Height
            End If
            Dim location As New Point(left, top)
            finalG.DrawImage(Image, location)
        Next
        Return finalBM
    End Function
    'This class the db Picturebox, is Double Buffered, for less flickering
    Public Class dbPictureBox
        Inherits PictureBox
        Sub New()
            Me.DoubleBuffered = True
        End Sub
    End Class
End Class
