import React from 'react';
import {
  Box,
  Paper,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableRow,
  CircularProgress,
  Card,
  CardMedia,
  CardContent
} from '@mui/material';

export default function ProductViewer({ product, productPath }) {
  console.log('ProductViewer props:', { product, productPath });

  if (!product) {
    console.log('No product data available');
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', p: 3 }}>
        <CircularProgress />
      </Box>
    );
  }

  if (product.type === 'error') {
    console.log('Product error:', product.error);
    return (
      <Paper sx={{ p: 2, textAlign: 'center' }}>
        <Typography color="error">
          {product.error || 'Failed to parse product data'}
        </Typography>
      </Paper>
    );
  }

  console.log('Rendering product data:', product);

  // Handle image display (PNG or BMP)
  if (product.type === 'image/png' || product.type === 'image/bmp') {
    return (
      <Box sx={{ width: '100%' }}>
        <Typography variant="subtitle2" color="text.secondary" gutterBottom>
          ID: {product.id}
        </Typography>
        <Card>
          <CardMedia
            component="img"
            image={product.data}
            alt="Product Image"
            sx={{ 
              maxHeight: '500px',
              objectFit: 'contain',
              backgroundColor: 'background.paper'
            }}
          />
          <CardContent>
            <Table size="small">
              <TableBody>
                <TableRow>
                  <TableCell component="th" scope="row">Type</TableCell>
                  <TableCell>{product.formatted_type}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Dimensions</TableCell>
                  <TableCell>{product.width} × {product.height}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Mode</TableCell>
                  <TableCell>{product.mode}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Path</TableCell>
                  <TableCell sx={{ wordBreak: 'break-all' }}>{productPath}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </Box>
    );
  }

  // Handle Text display
  if (product.type === 'Text') {
    return (
      <Box sx={{ width: '100%' }}>
        <Typography variant="subtitle2" color="text.secondary" gutterBottom>
          ID: {product.id}
        </Typography>
        <Card>
          <CardContent>
            <Typography variant="h6" gutterBottom>Text Content</Typography>
            <Paper 
              variant="outlined" 
              sx={{ 
                p: 2, 
                backgroundColor: 'background.default',
                fontFamily: 'monospace',
                whiteSpace: 'pre-wrap',
                overflowWrap: 'break-word'
              }}
            >
              {product.data}
            </Paper>
            <Table size="small" sx={{ mt: 2 }}>
              <TableBody>
                <TableRow>
                  <TableCell component="th" scope="row">Type</TableCell>
                  <TableCell>{product.formatted_type}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Path</TableCell>
                  <TableCell sx={{ wordBreak: 'break-all' }}>{productPath}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </Box>
    );
  }

  // Handle Reserved type
  if (product.type === 'Reserved') {
    return (
      <Box sx={{ width: '100%' }}>
        <Typography variant="subtitle2" color="text.secondary" gutterBottom>
          ID: {product.id}
        </Typography>
        <Card>
          <CardContent>
            <Typography variant="h6" gutterBottom>Reserved Product</Typography>
            <Table size="small">
              <TableBody>
                <TableRow>
                  <TableCell component="th" scope="row">Type</TableCell>
                  <TableCell>{product.formatted_type}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Path</TableCell>
                  <TableCell sx={{ wordBreak: 'break-all' }}>{productPath}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </Box>
    );
  }

  // Handle Raw type
  if (product.type === 'Raw') {
    return (
      <Box sx={{ width: '100%' }}>
        <Typography variant="subtitle2" color="text.secondary" gutterBottom>
          ID: {product.id}
        </Typography>
        <TableContainer component={Paper}>
          <Table>
            <TableBody>
              <TableRow>
                <TableCell component="th" scope="row">Type</TableCell>
                <TableCell>{product.formatted_type}</TableCell>
              </TableRow>
              {product.value && (
                <TableRow>
                  <TableCell component="th" scope="row">Value</TableCell>
                  <TableCell>{product.value}</TableCell>
                </TableRow>
              )}
              {product.hex && (
                <TableRow>
                  <TableCell component="th" scope="row">Hexadecimal</TableCell>
                  <TableCell>{product.hex}</TableCell>
                </TableRow>
              )}
              {product.binary && (
                <TableRow>
                  <TableCell component="th" scope="row">Binary</TableCell>
                  <TableCell sx={{ fontFamily: 'monospace', wordBreak: 'break-all' }}>
                    {product.binary}
                  </TableCell>
                </TableRow>
              )}
              <TableRow>
                <TableCell component="th" scope="row">Path</TableCell>
                <TableCell sx={{ wordBreak: 'break-all' }}>{productPath}</TableCell>
              </TableRow>
            </TableBody>
          </Table>
        </TableContainer>
      </Box>
    );
  }

  // Default fallback for other types
  return (
    <Box sx={{ width: '100%' }}>
      <Typography variant="subtitle2" color="text.secondary" gutterBottom>
        ID: {product.id}
      </Typography>
      <TableContainer component={Paper}>
        <Table>
          <TableBody>
            <TableRow>
              <TableCell component="th" scope="row">Type</TableCell>
              <TableCell>{product.formatted_type || product.type}</TableCell>
            </TableRow>
            {Object.entries(product).map(([key, value]) => {
              if (key !== 'id' && key !== 'type' && key !== 'formatted_type' && 
                  key !== 'data' && typeof value !== 'object') {
                return (
                  <TableRow key={key}>
                    <TableCell component="th" scope="row">{key}</TableCell>
                    <TableCell sx={{ wordBreak: 'break-all' }}>{String(value)}</TableCell>
                  </TableRow>
                );
              }
              return null;
            })}
            <TableRow>
              <TableCell component="th" scope="row">Path</TableCell>
              <TableCell sx={{ wordBreak: 'break-all' }}>{productPath}</TableCell>
            </TableRow>
          </TableBody>
        </Table>
      </TableContainer>
    </Box>
  );
}
